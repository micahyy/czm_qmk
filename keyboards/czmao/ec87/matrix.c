/* Copyright 2026 micahyy
 *
 * EC87 capacitive matrix — RC charge-timing v2 (DEBUG)
 *
 * Improvements over v1:
 *  - Mux enable stays on; settle delay after channel switch
 *  - 5 samples per key, median filter (kills outliers like 52, 392)
 *  - Longer discharge (10 µs)
 *  - Threshold raised to 120 cycles
 *  - Baseline averaged over 64 samples
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
#define THRESHOLD_DELTA      120
#define DISCHARGE_US         10
#define MUX_SETTLE_US        2
#define SAMPLES_PER_KEY      5
#define BASELINE_SAMPLES     64
#define TIMEOUT_CYCLES       8000
#define DEBUG_PRINT_INTERVAL 15

/* ── STM32F103 registers ─────────────────────────────────────────────── */
#define EC_RCC_APB2ENR  (*(volatile uint32_t *)0x40021018)
#define EC_GPIOA_CRL    (*(volatile uint32_t *)0x40010800)
#define EC_GPIOA_BSRR   (*(volatile uint32_t *)0x40010810)
#define EC_GPIOA_IDR    (*(volatile uint32_t *)0x40010808)
#define EC_GPIOA_ODR    (*(volatile uint32_t *)0x4001080C)

#define EC_PA1_MASK     0x000000F0u
#define EC_ROW_MASK     0xFFFFFF00u

#define EC_DEMCR        (*(volatile uint32_t *)0xE000EDFC)
#define EC_DWT_CYCCNT   (*(volatile uint32_t *)0xE0001004)
#define EC_DWT_CTRL     (*(volatile uint32_t *)0xE0001000)

/* ── State ───────────────────────────────────────────────────────────── */
static uint16_t baseline[ROW_COUNT][COL_COUNT];
static uint16_t cap_raw[ROW_COUNT][COL_COUNT];
static int16_t  cap_diff[ROW_COUNT][COL_COUNT];
static uint8_t  scan_counter = 0;

/* ── PA1 ─────────────────────────────────────────────────────────────── */

static inline void pa1_analog(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK);
}

static inline void pa1_drive_low(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 17);
}

static inline void pa1_input_pullup(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x8u << 4);
    EC_GPIOA_ODR |= (1u << 1);
}

/* ── Rows PA2-PA7 ────────────────────────────────────────────────────── */

static inline void rows_all_floating(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_ROW_MASK) | (0x44444400u);
}

static inline void row_set_output_low(uint8_t row) {
    rows_all_floating();
    uint8_t shift = 8 + row * 4;
    EC_GPIOA_CRL &= ~(0xFu << shift);
    EC_GPIOA_CRL |=  (0x3u << shift);
    EC_GPIOA_BSRR = (1u << (18 + row));
}

/* ── Mux ─────────────────────────────────────────────────────────────── */

static void mux_set_channel(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

static void mux_enable(void)  { gpio_write_pin(MUX_EN_PIN, 0); }
static void mux_disable(void) { gpio_write_pin(MUX_EN_PIN, 1); }

/* ── DWT ─────────────────────────────────────────────────────────────── */

static void dwt_init(void) {
    EC_DEMCR |= (1u << 24);
    EC_DWT_CYCCNT = 0;
    EC_DWT_CTRL |= (1u << 0);
}

/* ── Median filter for 5 samples ─────────────────────────────────────── */

static int16_t median5(int16_t a, int16_t b, int16_t c, int16_t d, int16_t e) {
    /* Simple sorting network for 5 elements */
    int16_t t;
    if (a > b) { t = a; a = b; b = t; }
    if (d > e) { t = d; d = e; e = t; }
    if (a > d) { t = a; a = d; d = t; t = b; b = e; e = t; }
    if (b > c) { t = b; b = c; c = t; }
    if (d > c) { t = d; d = c; c = t; }
    if (b > d) { t = b; b = d; d = t; }
    return d;  /* median */
}

/* ── Single charge-time measurement ──────────────────────────────────── */

static inline uint16_t measure_once(uint8_t row) {
    /* Discharge */
    row_set_output_low(row);
    pa1_drive_low();
    wait_us(DISCHARGE_US);

    /* Start charging via internal pull-up, count cycles */
    pa1_input_pullup();
    uint32_t start = EC_DWT_CYCCNT;
    while (!(EC_GPIOA_IDR & (1u << 1))) {
        if ((uint32_t)(EC_DWT_CYCCNT - start) > TIMEOUT_CYCLES) return TIMEOUT_CYCLES;
    }
    return (uint16_t)(EC_DWT_CYCCNT - start);
}

/* ── Averaged capacitance read ───────────────────────────────────────── */

static uint16_t measure_cap(uint8_t row, uint8_t col) {
    mux_set_channel(col);
    wait_us(MUX_SETTLE_US);

    int16_t s[SAMPLES_PER_KEY];
    for (int i = 0; i < SAMPLES_PER_KEY; i++)
        s[i] = (int16_t)measure_once(row);

    pa1_analog();
    return (uint16_t)median5(s[0], s[1], s[2], s[3], s[4]);
}

/* ── Baseline ────────────────────────────────────────────────────────── */

static void calibrate_baseline(void) {
    mux_enable();
    wait_us(100);  /* let mux fully turn on */

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint32_t sum = 0;
            for (int s = 0; s < BASELINE_SAMPLES; s++)
                sum += measure_cap(row, col);
            baseline[row][col] = (uint16_t)(sum / BASELINE_SAMPLES);
        }
    }
    rows_all_floating();
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
            if (v > max_raw && v < TIMEOUT_CYCLES) { max_raw = v; mr_r = r; mr_c = c; }
            if (v < min_raw) min_raw = v;
            if (d > max_diff) { max_diff = d; md_r = r; md_c = c; }
        }
    }

    xprintf("EC87v2 min=%u max=%u R%uC%u | Dmax=%d R%uC%u\n",
            min_raw, max_raw, mr_r, mr_c, max_diff, md_r, md_c);

    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%u:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++) {
            if (cap_raw[r][c] >= TIMEOUT_CYCLES)
                xprintf("  -- ");
            else
                xprintf(" %3u", cap_raw[r][c]);
        }
        print("\n");
    }
    print("DIFF:\n");
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%u:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++) {
            if (cap_raw[r][c] >= TIMEOUT_CYCLES)
                xprintf("  -- ");
            else
                xprintf(" %4d", cap_diff[r][c]);
        }
        print("\n");
    }
    print("\n");
}

/* ── QMK interface ───────────────────────────────────────────────────── */

void matrix_init_custom(void) {
    debug_enable = true;
    debug_matrix = true;
    wait_ms(200);

    EC_RCC_APB2ENR |= (1u << 2);
    dwt_init();

    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 0);  /* keep mux enabled */

    rows_all_floating();
    calibrate_baseline();

    print("\nEC87 RC-TIMING v2 READY (median5, thresh=120)\n\n");
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        matrix_row_t new_row = 0;

        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint16_t val = measure_cap(row, col);
            int16_t  diff;

            if (val >= TIMEOUT_CYCLES) {
                /* No key / open circuit at this position */
                cap_raw[row][col]  = TIMEOUT_CYCLES;
                cap_diff[row][col] = 0;
                continue;
            }

            diff = (int16_t)val - (int16_t)baseline[row][col];
            cap_raw[row][col]  = val;
            cap_diff[row][col] = diff;

            if (diff > THRESHOLD_DELTA)
                new_row |= (matrix_row_t)1 << col;

            /* Slow baseline tracking when not pressed */
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

    if (++scan_counter >= DEBUG_PRINT_INTERVAL) {
        scan_counter = 0;
        debug_print();
    }

    return changed;
}
