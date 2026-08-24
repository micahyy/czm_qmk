/* Copyright 2026 micahyy
 *
 * EC87 capacitive matrix — CHARGE REDISTRIBUTION v3.8
 *
 * v3.7 bug:
 *   PA1 stayed driven HIGH during ADC read → DC path through 74HC4067
 *   (~70Ω R_on) to row pin → ADC reads full VDD (~4082).
 *
 * v3.8 fix:
 *   - After charge phase, PA1 goes HIGH-Z (analog input) — proper charge
 *     redistribution; no DC path.
 *   - 1MΩ pull-down on COM: τ = 1MΩ × ~30pF = 30µs. ADC sample is 4.6µs,
 *     so droop during sample is <0.5 count — negligible.
 *   - Row also goes HIGH-Z; both sides float on C_key, voltage settles at
 *     VDD * C_key / (C_key + C_parasitic_row).
 *   - Slightly longer settle (2µs) for mux R_on + C to settle.
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
#define THRESHOLD_DELTA      200
#define DISCHARGE_US         5
#define CHARGE_US            5
#define SETTLE_US            2
#define BASELINE_SAMPLES     16
#define DEBUG_PRINT_INTERVAL 15

/* ── STM32F103 registers ─────────────────────────────────────────────── */
#define EC_RCC_APB2ENR  (*(volatile uint32_t *)0x40021018)
#define EC_RCC_CFGR     (*(volatile uint32_t *)0x40021004)

#define EC_ADC1_SR      (*(volatile uint32_t *)0x40012400)
#define EC_ADC1_CR1     (*(volatile uint32_t *)0x40012404)
#define EC_ADC1_CR2     (*(volatile uint32_t *)0x40012408)
#define EC_ADC1_SMPR2   (*(volatile uint32_t *)0x40012410)
#define EC_ADC1_SQR1    (*(volatile uint32_t *)0x40012428)
#define EC_ADC1_SQR3    (*(volatile uint32_t *)0x4001242C)
#define EC_ADC1_DR      (*(volatile uint32_t *)0x4001244C)

#define EC_GPIOA_CRL    (*(volatile uint32_t *)0x40010800)
#define EC_GPIOA_BSRR   (*(volatile uint32_t *)0x40010810)

#define EC_PA1_MASK     0x000000F0u
#define EC_ROW_MASK     0xFFFFFF00u

/* ── State ───────────────────────────────────────────────────────────── */
static uint16_t baseline[ROW_COUNT][COL_COUNT];
static uint16_t cap_raw[ROW_COUNT][COL_COUNT];
static int16_t  cap_diff[ROW_COUNT][COL_COUNT];
static uint8_t  scan_counter = 0;

/* ── PA1 modes ───────────────────────────────────────────────────────── */

/* PA1 = push-pull output LOW  (CNF=00, MODE=11 → 0x3, shift 4) */
static inline void pa1_low(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 17);
}

/* PA1 = push-pull output HIGH */
static inline void pa1_high(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 1);
}

/* PA1 = analog input (HIGH-Z, CNF=00, MODE=00 → 0x0) */
static inline void pa1_hiz(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK);
}

/* ── Rows ────────────────────────────────────────────────────────────── */

static inline void all_rows_low(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_ROW_MASK) | (0x33333300u);
    EC_GPIOA_BSRR = (0x3Fu << 18);
}

/* One row pin → analog input (high-Z); CNF=00 MODE=00 */
static inline void row_analog(uint8_t row) {
    uint8_t shift = (row + 2) * 4;
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~(0xFu << shift));
}

/* ── Mux ─────────────────────────────────────────────────────────────── */

static void mux_set(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

/* ── ADC ─────────────────────────────────────────────────────────────── */

static void adc_init(void) {
    EC_RCC_APB2ENR |= (1u << 2) | (1u << 3) | (1u << 9);
    EC_RCC_CFGR = (EC_RCC_CFGR & ~(3u << 14)) | (2u << 14);

    EC_ADC1_CR2 = (1u << 0);
    wait_ms(2);

    EC_ADC1_CR2 |= (1u << 3);
    { volatile uint32_t t = 50000; while ((EC_ADC1_CR2 & (1u << 3)) && --t); }
    EC_ADC1_CR2 |= (1u << 2);
    { volatile uint32_t t = 200000; while ((EC_ADC1_CR2 & (1u << 2)) && --t); }

    EC_ADC1_CR1 = 0;
    EC_ADC1_CR2 = (1u << 0) | (1u << 20) | (7u << 17);

    /* 55.5 cycle sample time for ALL channels */
    EC_ADC1_SMPR2 = 0x2DB6DB6D;

    EC_ADC1_SQR1 = 0;
}

static inline uint16_t adc_read_channel(uint8_t ch) {
    EC_ADC1_SQR3 = ch;
    EC_ADC1_CR2 |= (1u << 22);
    volatile uint32_t t = 100000;
    while (!(EC_ADC1_SR & (1u << 1)) && --t);
    return (uint16_t)(EC_ADC1_DR & 0xFFFu);
}

/* ── Measure one key ─────────────────────────────────────────────────── */

static uint16_t measure_key(uint8_t col, uint8_t row) {
    uint8_t adc_ch = row + 2;

    /* Phase 1: discharge — PA1 LOW, all rows LOW */
    pa1_low();
    all_rows_low();
    wait_us(DISCHARGE_US);

    /* Phase 2: charge — PA1 drives column HIGH through mux,
     * target row held LOW to put VDD across C_key */
    pa1_high();
    wait_us(CHARGE_US);

    /* Phase 3: release BOTH sides to high-Z.
     * - row → analog (floating)
     * - PA1 → analog (floating)
     * Charge on C_key redistributes between C_key and row parasitic.
     * No DC path; 1MΩ pull-down droop is negligible in 4.6µs. */
    row_analog(row);
    pa1_hiz();

    /* Phase 4: settle through mux R_on (~70Ω) */
    wait_us(SETTLE_US);

    /* Phase 5: ADC read row voltage */
    uint16_t val = adc_read_channel(adc_ch);

    /* Phase 6: discharge */
    pa1_low();
    all_rows_low();

    return val;
}

/* ── Baseline ────────────────────────────────────────────────────────── */

static void calibrate_baseline(void) {
    gpio_write_pin(MUX_EN_PIN, 0);
    wait_us(100);

    for (uint8_t col = 0; col < COL_COUNT; col++) {
        mux_set(col);
        for (uint8_t row = 0; row < ROW_COUNT; row++) {
            uint32_t sum = 0;
            for (int s = 0; s < BASELINE_SAMPLES; s++)
                sum += measure_key(col, row);
            baseline[row][col] = (uint16_t)(sum / BASELINE_SAMPLES);
        }
    }
}

/* ── Debug ───────────────────────────────────────────────────────────── */

static void debug_print(void) {
    uint16_t max_raw = 0, min_raw = 4095;
    int16_t  max_diff = -4096;
    uint8_t  mr_r = 0, mr_c = 0, md_r = 0, md_c = 0;

    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        for (uint8_t c = 0; c < COL_COUNT; c++) {
            if (cap_raw[r][c] > max_raw) { max_raw = cap_raw[r][c]; mr_r = r; mr_c = c; }
            if (cap_raw[r][c] < min_raw) min_raw = cap_raw[r][c];
            if (cap_diff[r][c] > max_diff) { max_diff = cap_diff[r][c]; md_r = r; md_c = c; }
        }
    }

    xprintf("EC87v38 adc %u-%u @R%dC%d | Dmax=%d @R%dC%d thr=%d\n",
            min_raw, max_raw, mr_r, mr_c, max_diff, md_r, md_c, THRESHOLD_DELTA);

    print("RAW:\n");
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%d:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++)
            xprintf(" %4u", cap_raw[r][c]);
        print("\n");
    }
    print("DIFF:\n");
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%d:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++)
            xprintf(" %4d", cap_diff[r][c]);
        print("\n");
    }
    print("\n");
}

/* ── QMK ─────────────────────────────────────────────────────────────── */

void matrix_init_custom(void) {
    debug_enable = true;
    debug_matrix = true;

    print("\nEC87 v3.8 booting (PA1 hi-Z after charge)...\n");

    EC_RCC_APB2ENR |= (1u << 2) | (1u << 3);

    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 1);

    adc_init();
    print("ADC ready (55.5cyc, PA1 hi-Z after charge)\n");

    gpio_write_pin(MUX_EN_PIN, 0);
    print("Calibrating...\n");
    calibrate_baseline();
    all_rows_low();
    gpio_write_pin(MUX_EN_PIN, 1);

    print("EC87 v3.8 READY\n\n");
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    gpio_write_pin(MUX_EN_PIN, 0);

    for (uint8_t row = 0; row < ROW_COUNT; row++)
        current_matrix[row] = 0;

    for (uint8_t col = 0; col < COL_COUNT; col++) {
        mux_set(col);

        for (uint8_t row = 0; row < ROW_COUNT; row++) {
            uint16_t v = measure_key(col, row);
            int16_t  diff = (int16_t)v - (int16_t)baseline[row][col];

            cap_raw[row][col]  = v;
            cap_diff[row][col] = diff;

            if (diff > THRESHOLD_DELTA)
                current_matrix[row] |= (matrix_row_t)1 << col;

            if (diff < THRESHOLD_DELTA) {
                if      (v > baseline[row][col]) baseline[row][col] += 2;
                else if (v < baseline[row][col]) baseline[row][col] -= 2;
            }
        }
    }

    all_rows_low();
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
