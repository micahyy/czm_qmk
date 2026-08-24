/* Copyright 2026 micahyy
 *
 * EC87 capacitive matrix — ROW RC-CHARGE TIMING v3.2 (DEBUG)
 *
 * Findings from v3.1: driving column HIGH and ADC-sensing rows produced
 * 4080-4082 on EVERY row/column → rows have external pull-up resistors
 * that pull them to VDD within microseconds, drowning any capacitive
 * coupling signal in the ADC sample window.
 *
 * New approach — use the pull-ups as the timing resistor:
 *   For each column (via 74HC4067):
 *     1. PA1 drives selected column LOW (key cap references GND)
 *     2. All PA2-PA7 driven LOW to discharge
 *     3. Switch PA2-PA7 to floating input
 *     4. External pull-ups charge each row; DWT polls all 6 simultaneously
 *     5. Record CPU cycles for each row to cross VIH threshold
 *   Pressed key → larger C_key → longer charge time → more cycles
 *
 * The selected column at LOW means that position's key cap adds C to GND.
 * Non-selected columns are floating (mux disconnected) → their caps don't
 * contribute to the RC constant.
 */

#include "matrix.h"
#include "gpio.h"
#include "wait.h"
#include "print.h"
#include "debug.h"
#include <stdint.h>

#define ROW_COUNT 6
#define COL_COUNT 16

static const pin_t mux_pins[4] = {B8, B9, B10, B11};
#define MUX_EN_PIN B12

/* ── Tuning ──────────────────────────────────────────────────────────── */
#define THRESHOLD_DELTA      8
#define DISCHARGE_US         20
#define MUX_SETTLE_US        2
#define SAMPLES_PER_KEY      5
#define BASELINE_SAMPLES     16
#define TIMEOUT_CYCLES       20000
#define DEBUG_PRINT_INTERVAL 20

/* ── STM32F103 registers ─────────────────────────────────────────────── */
#define EC_RCC_APB2ENR  (*(volatile uint32_t *)0x40021018)

#define EC_GPIOA_CRL    (*(volatile uint32_t *)0x40010800)
#define EC_GPIOA_BSRR   (*(volatile uint32_t *)0x40010810)
#define EC_GPIOA_IDR    (*(volatile uint32_t *)0x40010808)

#define EC_PA1_MASK     0x000000F0u
#define EC_ROW_MASK     0xFFFFFF00u

/* DWT */
#define EC_DEMCR        (*(volatile uint32_t *)0xE000EDFC)
#define EC_DWT_CTRL     (*(volatile uint32_t *)0xE0001000)
#define EC_DWT_CYCCNT   (*(volatile uint32_t *)0xE0001004)

/* ── State ───────────────────────────────────────────────────────────── */
static uint16_t baseline[ROW_COUNT][COL_COUNT];
static uint16_t cap_raw[ROW_COUNT][COL_COUNT];
static int16_t  cap_diff[ROW_COUNT][COL_COUNT];
static uint8_t  scan_counter = 0;

/* ── DWT ─────────────────────────────────────────────────────────────── */

static void dwt_init(void) {
    EC_DEMCR |= (1u << 24);
    EC_DWT_CYCCNT = 0;
    EC_DWT_CTRL |= (1u << 0);
}

static inline uint32_t dwt_now(void) {
    return EC_DWT_CYCCNT;
}

/* ── PA1 (COM / column drive) ────────────────────────────────────────── */

static inline void pa1_low(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 17);  /* BR1 */
}

/* ── Row pins PA2-PA7 ────────────────────────────────────────────────── */

static inline void rows_discharge(void) {
    /* Push-pull 50MHz output, all LOW */
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_ROW_MASK) | (0x33333300u);
    EC_GPIOA_BSRR = (0x3Fu << 18);  /* BR2..BR7 */
}

static inline void rows_float(void) {
    /* Floating input: CNF=01, MODE=00 for PA2-PA7 */
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_ROW_MASK) | (0x44444400u);
}

/* ── Mux ─────────────────────────────────────────────────────────────── */

static void mux_set(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

/* ── Measure one column: 6 rows simultaneously ──────────────────────── */

static void measure_column(uint8_t col, uint16_t *times) {
    uint16_t samples[SAMPLES_PER_KEY][ROW_COUNT];

    for (int s = 0; s < SAMPLES_PER_KEY; s++) {
        mux_set(col);
        pa1_low();
        rows_discharge();
        wait_us(DISCHARGE_US);

        rows_float();
        wait_us(MUX_SETTLE_US);

        uint32_t t0 = dwt_now();
        uint8_t remaining = 0x3Fu;
        uint16_t t[6];
        for (int r = 0; r < 6; r++) t[r] = TIMEOUT_CYCLES;

        while (remaining) {
            uint32_t elapsed = dwt_now() - t0;
            if ((int32_t)elapsed > TIMEOUT_CYCLES) break;

            uint8_t pins = (uint8_t)((EC_GPIOA_IDR >> 2) & 0x3Fu);
            uint8_t ready = pins & remaining;
            if (ready) {
                for (int r = 0; r < 6; r++) {
                    if (ready & (1u << r)) {
                        t[r] = (uint16_t)elapsed;
                        remaining &= (uint8_t)~(1u << r);
                    }
                }
            }
        }
        for (int r = 0; r < 6; r++) samples[s][r] = t[r];
    }

    /* Median filter for each row */
    for (int r = 0; r < 6; r++) {
        /* Simple 5-point median: sort and pick middle */
        uint16_t a[5];
        for (int s = 0; s < SAMPLES_PER_KEY; s++) a[s] = samples[s][r];
        /* Insertion sort */
        for (int i = 1; i < SAMPLES_PER_KEY; i++) {
            uint16_t key = a[i];
            int j = i - 1;
            while (j >= 0 && a[j] > key) { a[j+1] = a[j]; j--; }
            a[j+1] = key;
        }
        times[r] = a[SAMPLES_PER_KEY / 2];
    }
}

/* ── Baseline ────────────────────────────────────────────────────────── */

static void calibrate_baseline(void) {
    gpio_write_pin(MUX_EN_PIN, 0);
    wait_us(100);

    uint16_t times[6];
    for (uint8_t col = 0; col < COL_COUNT; col++) {
        uint32_t sums[6] = {0};
        for (int s = 0; s < BASELINE_SAMPLES; s++) {
            measure_column(col, times);
            for (int r = 0; r < 6; r++) sums[r] += times[r];
        }
        for (int r = 0; r < 6; r++)
            baseline[r][col] = (uint16_t)(sums[r] / BASELINE_SAMPLES);
    }
}

/* ── Debug ───────────────────────────────────────────────────────────── */

static void debug_print(void) {
    uint16_t max_raw = 0, min_raw = 65535;
    int16_t  max_diff = -32768;
    uint8_t  mr_r = 0, mr_c = 0, md_r = 0, md_c = 0;

    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        for (uint8_t c = 0; c < COL_COUNT; c++) {
            if (cap_raw[r][c] > max_raw) { max_raw = cap_raw[r][c]; mr_r = r; mr_c = c; }
            if (cap_raw[r][c] < min_raw) min_raw = cap_raw[r][c];
            if (cap_diff[r][c] > max_diff) { max_diff = cap_diff[r][c]; md_r = r; md_c = c; }
        }
    }

    xprintf("EC87v32 cyc %u-%u @R%dC%d | Dmax=%d @R%dC%d thr=%d\n",
            min_raw, max_raw, mr_r, mr_c, max_diff, md_r, md_c, THRESHOLD_DELTA);

    print("RAW:\n");
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%d:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++)
            xprintf(" %5u", cap_raw[r][c]);
        print("\n");
    }
    print("DIFF:\n");
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%d:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++)
            xprintf(" %5d", cap_diff[r][c]);
        print("\n");
    }
    print("\n");
}

/* ── QMK ─────────────────────────────────────────────────────────────── */

void matrix_init_custom(void) {
    debug_enable = true;
    debug_matrix = true;

    print("\nEC87 v3.2 booting...\n");

    EC_RCC_APB2ENR |= (1u << 2) | (1u << 3);  /* GPIOA + GPIOB */

    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 1);

    dwt_init();
    print("DWT ready\n");

    gpio_write_pin(MUX_EN_PIN, 0);
    print("Calibrating baseline...\n");
    calibrate_baseline();
    gpio_write_pin(MUX_EN_PIN, 1);

    print("EC87 v3.2 READY - row RC timing, ext pull-up as R\n\n");
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;
    uint16_t times[6];

    gpio_write_pin(MUX_EN_PIN, 0);

    for (uint8_t row = 0; row < ROW_COUNT; row++)
        current_matrix[row] = 0;

    for (uint8_t col = 0; col < COL_COUNT; col++) {
        measure_column(col, times);

        for (uint8_t row = 0; row < ROW_COUNT; row++) {
            uint16_t v = times[row];
            int16_t  diff = (int16_t)v - (int16_t)baseline[row][col];

            cap_raw[row][col]  = v;
            cap_diff[row][col] = diff;

            if (diff > THRESHOLD_DELTA)
                current_matrix[row] |= (matrix_row_t)1 << col;

            if (diff < THRESHOLD_DELTA) {
                if      (v > baseline[row][col]) baseline[row][col]++;
                else if (v < baseline[row][col]) baseline[row][col]--;
            }
        }
    }

    gpio_write_pin(MUX_EN_PIN, 1);

    static matrix_row_t prev[ROW_COUNT];
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        if (current_matrix[r] != prev[r]) {
            changed = true;
            prev[r] = current_matrix[r];
        }
    }

    if (++scan_counter >= DEBUG_PRINT_INTERVAL) {
        scan_counter = 0;
        debug_print();
    }

    return changed;
}
