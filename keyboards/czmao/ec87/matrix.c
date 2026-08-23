/* Copyright 2026 micahyy
 *
 * EC87 capacitive matrix — RC charge-timing method (DEBUG)
 *
 * Instead of ADC, measure the time for PA1 (COM) to charge through
 * the STM32 internal pull-up (~40 kΩ).  Pressed key adds capacitance
 * → longer rise time.
 *
 * PA1  : push-pull LOW to discharge, then input pull-up to time
 * Rows : active row = push-pull LOW (key cap to GND), others floating
 * 74HC4067 selects column.
 */

#include "matrix.h"
#include "gpio.h"
#include "wait.h"
#include "print.h"
#include <stdint.h>

extern uint8_t debug_enable;

#define ROW_COUNT 6
#define COL_COUNT 16

static const pin_t mux_pins[4] = {B8, B9, B10, B11};
#define MUX_EN_PIN B12

/* ── Tuning ──────────────────────────────────────────────────────────── */
#define THRESHOLD_DELTA     25    /* cycles above baseline = pressed */
#define DISCHARGE_US        3
#define BASELINE_SAMPLES    32
#define TIMEOUT_CYCLES      5000
#define DEBUG_PRINT_INTERVAL 20

/* ── STM32F103 registers ─────────────────────────────────────────────── */
#define EC_RCC_APB2ENR  (*(volatile uint32_t *)0x40021018)

#define EC_GPIOA_CRL    (*(volatile uint32_t *)0x40010800)
#define EC_GPIOA_BSRR   (*(volatile uint32_t *)0x40010810)
#define EC_GPIOA_IDR    (*(volatile uint32_t *)0x40010808)
#define EC_GPIOA_ODR    (*(volatile uint32_t *)0x4001080C)

#define EC_PA1_MASK     0x000000F0u
#define EC_ROW_MASK     0xFFFFFF00u  /* PA2..PA7 = CRL bits 8-31 */

/* DWT cycle counter (Cortex-M3) */
#define EC_DEMCR        (*(volatile uint32_t *)0xE000EDFC)
#define EC_DWT_CYCCNT   (*(volatile uint32_t *)0xE0001004)
#define EC_DWT_CTRL     (*(volatile uint32_t *)0xE0001000)

/* ── State ───────────────────────────────────────────────────────────── */
static uint16_t baseline[ROW_COUNT][COL_COUNT];
static uint16_t cap_raw[ROW_COUNT][COL_COUNT];    /* charge time in CPU cycles */
static int16_t  cap_diff[ROW_COUNT][COL_COUNT];
static uint8_t  scan_counter = 0;

/* ── PA1 mode ────────────────────────────────────────────────────────── */

/* PA1: analog / high-Z (used during row switching) */
static inline void pa1_analog(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK);
}

/* PA1: push-pull output 50 MHz, drive LOW — discharges the node */
static inline void pa1_drive_low(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 17);  /* BR1 */
}

/* PA1: input with internal pull-up.
 * CNF=10 (pull-up/down input), MODE=00, then ODR bit1=1 selects pull-up. */
static inline void pa1_input_pullup(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x8u << 4);  /* CNF=10,MODE=00 */
    EC_GPIOA_ODR |= (1u << 1);   /* pull-up (not pull-down) */
}

/* ── Row pins (PA2-PA7 = CRL bits 8-31) ──────────────────────────────── */

static inline void rows_all_floating(void) {
    /* Floating input: CNF=01, MODE=00 for each pin → 0x4 per nibble */
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_ROW_MASK) | (0x44444400u);
}

static inline void row_set_output_low(uint8_t row) {
    rows_all_floating();
    uint8_t shift = 8 + row * 4;
    EC_GPIOA_CRL &= ~(0xFu << shift);
    EC_GPIOA_CRL |=  (0x3u << shift);  /* push-pull 50 MHz output */
    EC_GPIOA_BSRR = (1u << (18 + row)); /* BR: drive LOW */
}

/* ── Mux ─────────────────────────────────────────────────────────────── */

static void mux_set_channel(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

static void mux_enable(void)  { gpio_write_pin(MUX_EN_PIN, 0); }
static void mux_disable(void) { gpio_write_pin(MUX_EN_PIN, 1); }

/* ── DWT cycle counter ───────────────────────────────────────────────── */

static void dwt_init(void) {
    EC_DEMCR |= (1u << 24);           /* TRCENA */
    EC_DWT_CYCCNT = 0;
    EC_DWT_CTRL |= (1u << 0);         /* CYCCNTENA */
}

/* ── Capacitance measurement (RC charge time) ────────────────────────── */

static uint16_t measure_cap(uint8_t row, uint8_t col) {
    mux_set_channel(col);

    /* Phase 1: discharge. PA1 = PP LOW, active row = PP LOW. */
    row_set_output_low(row);
    pa1_drive_low();
    wait_us(DISCHARGE_US);

    /* Phase 2: switch PA1 to pull-up input, start counting.
     * The internal ~40 kΩ pull-up charges the node through mux Ron.
     * Pressed key adds C → takes longer to reach V_IH. */
    pa1_input_pullup();

    uint32_t start = EC_DWT_CYCCNT;
    /* Spin until PA1 reads HIGH (V_IH ≈ 0.7 × VDD ≈ 2.3 V) */
    while (!(EC_GPIOA_IDR & (1u << 1))) {
        if ((EC_DWT_CYCCNT - start) > TIMEOUT_CYCLES) break;
    }
    uint32_t elapsed = EC_DWT_CYCCNT - start;

    pa1_analog();
    return (uint16_t)elapsed;
}

/* ── Baseline ────────────────────────────────────────────────────────── */

static void calibrate_baseline(void) {
    mux_enable();
    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint32_t sum = 0;
            for (int s = 0; s < BASELINE_SAMPLES; s++)
                sum += measure_cap(row, col);
            baseline[row][col] = (uint16_t)(sum / BASELINE_SAMPLES);
        }
    }
    rows_all_floating();
    mux_disable();
}

/* ── Debug print ─────────────────────────────────────────────────────── */

static void debug_print(void) {
    uint16_t max_raw = 0, min_raw = 65535;
    int16_t  max_diff = -32768;
    uint8_t  mr_r = 0, mr_c = 0, md_r = 0, md_c = 0;

    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        for (uint8_t c = 0; c < COL_COUNT; c++) {
            uint16_t v = cap_raw[r][c];
            int16_t  d = cap_diff[r][c];
            if (v > max_raw) { max_raw = v; mr_r = r; mr_c = c; }
            if (v < min_raw)   min_raw = v;
            if (d > max_diff) { max_diff = d; md_r = r; md_c = c; }
        }
    }

    xprintf("=== EC87 RC | cycles %u-%u @R%uC%u | maxdiff=%d @R%uC%u ===\n",
            min_raw, max_raw, mr_r, mr_c, max_diff, md_r, md_c);

    print("RAW(cycles):\n");
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%u:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++)
            xprintf(" %4u", cap_raw[r][c]);
        print("\n");
    }

    print("DIFF:\n");
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%u:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++)
            xprintf(" %4d", cap_diff[r][c]);
        print("\n");
    }
    print("\n");
}

/* ── QMK interface ───────────────────────────────────────────────────── */

void matrix_init_custom(void) {
    debug_enable = 1;
    wait_ms(200);

    /* Enable GPIOA clock */
    EC_RCC_APB2ENR |= (1u << 2);

    dwt_init();

    /* Mux pins */
    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 1);

    rows_all_floating();
    calibrate_baseline();

    print("\n\n=== EC87 RC-TIMING DEBUG READY ===\n");
    print("Charge-time in CPU cycles. Pressed = higher value (positive diff).\n\n");
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    mux_enable();

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        matrix_row_t new_row = 0;

        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint16_t val = measure_cap(row, col);
            int16_t  diff = (int16_t)val - (int16_t)baseline[row][col];

            cap_raw[row][col]  = val;
            cap_diff[row][col] = diff;

            if (diff > THRESHOLD_DELTA)
                new_row |= (matrix_row_t)1 << col;

            /* Slow baseline tracking */
            if (diff < THRESHOLD_DELTA) {
                if      (val > baseline[row][col]) baseline[row][col]++;
                else if (val < baseline[row][col]) baseline[row][col]--;
            }
        }

        if (new_row != current_matrix[row]) {
            current_matrix[row] = new_row;
            changed = true;
        }
    }

    rows_all_floating();
    mux_disable();

    if (++scan_counter >= DEBUG_PRINT_INTERVAL) {
        scan_counter = 0;
        debug_print();
    }

    return changed;
}
