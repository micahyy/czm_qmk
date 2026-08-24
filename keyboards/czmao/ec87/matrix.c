/* Copyright 2026 micahyy
 *
 * EC87 capacitive matrix — RELAXATION OSCILLATOR v3.4
 *
 * v3.3 single-cycle RC timing had only 4-5 quantization levels
 * (~119/185/297/413 cycles) causing noise-induced false triggers.
 *
 * This version repeats charge/discharge N times per key and measures
 * total DWT cycles. The capacitance delta is amplified N-fold and
 * per-cycle noise averages out.
 *
 * Per (column, row) pair:
 *   1. PA1 drives selected column LOW through 74HC4067
 *   2. Target row: push-pull LOW (discharge)
 *   3. Switch target row to internal pull-up input
 *   4. Poll IDR until pin goes HIGH
 *   5. Discharge (back to step 2), repeat N_OSC times
 *   6. Total elapsed cycles ∝ R_pullup * (C_par + C_key)
 *
 * Pressed key → larger C_key → longer per-cycle → more total cycles.
 * Non-target rows are held LOW (push-pull) to avoid crosstalk.
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
#define N_OSC                32     /* charge/discharge cycles per key   */
#define THRESHOLD_DELTA      400    /* total cycles (32x single-cycle)   */
#define DISCHARGE_NOP        3      /* nops after switching to discharge */
#define BASELINE_SAMPLES     8
#define TIMEOUT_CYCLES       200000 /* per-row total timeout             */
#define DEBUG_PRINT_INTERVAL 15

/* ── STM32F103 ───────────────────────────────────────────────────────── */
#define EC_RCC_APB2ENR  (*(volatile uint32_t *)0x40021018)
#define EC_GPIOA_CRL    (*(volatile uint32_t *)0x40010800)
#define EC_GPIOA_BSRR   (*(volatile uint32_t *)0x40010810)
#define EC_GPIOA_IDR    (*(volatile uint32_t *)0x40010808)
#define EC_GPIOA_ODR    (*(volatile uint32_t *)0x4001080C)

#define EC_PA1_MASK     0x000000F0u
#define EC_ROW_MASK     0xFFFFFF00u
#define EC_ROW_BITS     (0x3Fu << 2)

#define EC_DEMCR        (*(volatile uint32_t *)0xE000EDFC)
#define EC_DWT_CTRL     (*(volatile uint32_t *)0xE0001000)
#define EC_DWT_CYCCNT   (*(volatile uint32_t *)0xE0001004)

/* ── State ───────────────────────────────────────────────────────────── */
static uint16_t baseline[ROW_COUNT][COL_COUNT];
static uint16_t cap_raw[ROW_COUNT][COL_COUNT];
static int16_t  cap_diff[ROW_COUNT][COL_COUNT];
static uint8_t  scan_counter = 0;

/* ── Helpers ─────────────────────────────────────────────────────────── */

static inline void dwt_init(void) {
    EC_DEMCR  |= (1u << 24);
    EC_DWT_CYCCNT = 0;
    EC_DWT_CTRL |= (1u << 0);
}

static inline void pa1_low(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 17);
}

/* Set a single row pin to push-pull 50MHz output LOW; others stay as-is */
static inline void row_output_low(uint8_t row) {
    uint8_t shift = (row + 2) * 4;
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~(0xFu << shift)) | (0x3u << shift);
    EC_GPIOA_BSRR = (1u << (18 + row));  /* BRn */
}

/* Set a single row pin to input with internal pull-up */
static inline void row_pullup(uint8_t row) {
    uint8_t shift = (row + 2) * 4;
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~(0xFu << shift)) | (0x8u << shift);
    EC_GPIOA_ODR |= (1u << (row + 2));
}

/* All rows push-pull LOW */
static inline void all_rows_low(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_ROW_MASK) | (0x33333300u);
    EC_GPIOA_BSRR = (0x3Fu << 18);
}

/* All rows: push-pull LOW (used as idle state / shield) */
static inline void all_rows_shield(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_ROW_MASK) | (0x33333300u);
    EC_GPIOA_BSRR = (0x3Fu << 18);
}

static void mux_set(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

/* ── Relaxation oscillator measurement for one key ───────────────────── */

static uint16_t measure_key(uint8_t col, uint8_t row) {
    uint32_t pin_bit = 1u << (row + 2);
    uint32_t t0 = EC_DWT_CYCCNT;
    uint32_t timeout = t0 + TIMEOUT_CYCLES;

    for (int osc = 0; osc < N_OSC; osc++) {
        /* Discharge phase */
        row_output_low(row);
        __asm__ __volatile__("nop\nnop\nnop\n");

        /* Charge phase: switch to pull-up input */
        row_pullup(row);

        /* Poll until HIGH or timeout */
        while (!(EC_GPIOA_IDR & pin_bit)) {
            if ((int32_t)(EC_DWT_CYCCNT - timeout) > 0)
                return TIMEOUT_CYCLES;
        }
    }
    return (uint16_t)(EC_DWT_CYCCNT - t0);
}

/* ── Baseline ────────────────────────────────────────────────────────── */

static void calibrate_baseline(void) {
    gpio_write_pin(MUX_EN_PIN, 0);
    wait_us(100);

    for (uint8_t col = 0; col < COL_COUNT; col++) {
        mux_set(col);
        pa1_low();
        all_rows_low();
        wait_us(5);

        for (uint8_t row = 0; row < ROW_COUNT; row++) {
            uint32_t sum = 0;
            for (int s = 0; s < BASELINE_SAMPLES; s++)
                sum += measure_key(col, row);
            baseline[row][col] = (uint16_t)(sum / BASELINE_SAMPLES);
            all_rows_low();
        }
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

    xprintf("EC87v34 cyc %u-%u @R%dC%d | Dmax=%d @R%dC%d thr=%d n=%d\n",
            min_raw, max_raw, mr_r, mr_c, max_diff, md_r, md_c, THRESHOLD_DELTA, N_OSC);

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

    print("\nEC87 v3.4 booting (relaxation oscillator x32)...\n");

    EC_RCC_APB2ENR |= (1u << 2) | (1u << 3);

    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 1);

    dwt_init();
    print("DWT ready\n");

    gpio_write_pin(MUX_EN_PIN, 0);
    print("Calibrating...\n");
    calibrate_baseline();
    gpio_write_pin(MUX_EN_PIN, 1);

    print("EC87 v3.4 READY\n\n");
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    gpio_write_pin(MUX_EN_PIN, 0);

    for (uint8_t row = 0; row < ROW_COUNT; row++)
        current_matrix[row] = 0;

    for (uint8_t col = 0; col < COL_COUNT; col++) {
        mux_set(col);
        pa1_low();

        for (uint8_t row = 0; row < ROW_COUNT; row++) {
            all_rows_low();
            uint16_t v = measure_key(col, row);
            int16_t  diff = (int16_t)v - (int16_t)baseline[row][col];

            cap_raw[row][col]  = v;
            cap_diff[row][col] = diff;

            if (diff > THRESHOLD_DELTA)
                current_matrix[row] |= (matrix_row_t)1 << col;

            /* Slow baseline auto-track when not pressed */
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
