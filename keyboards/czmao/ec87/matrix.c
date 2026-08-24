/* Copyright 2026 micahyy
 *
 * EC87 capacitive matrix — DRIVE-COLUMN / SENSE-ROW v3.1 (DEBUG)
 *
 * PA1 (COM through 74HC4067) = push-pull output, drives selected column
 * PA2-PA7 = ADC1_IN2..IN7, analog inputs sensing the 6 rows
 *
 * Fixes vs v3:
 *  - ADC CR1 SCAN bit set (was 0 → only ch2 converted, other 5 timed out)
 *  - Calibration loops have timeout
 *  - Baseline samples reduced 32→8 for fast startup
 *  - Early boot print to confirm firmware alive
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
#define SETTLE_US            3
#define BASELINE_SAMPLES     8
#define DEBUG_PRINT_INTERVAL 20
#define ADC_TIMEOUT          20000

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

/* ── PA1 (COM) mode ──────────────────────────────────────────────────── */

static inline void pa1_output_low(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 17);
}

static inline void pa1_output_high(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 1);
}

/* ── Row pins PA2-PA7 ────────────────────────────────────────────────── */

static inline void rows_discharge(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_ROW_MASK) | (0x33333300u);
    EC_GPIOA_BSRR = (0x3Fu << 18);
}

static inline void rows_sense(void) {
    EC_GPIOA_CRL &= ~EC_ROW_MASK;
}

/* ── Mux ─────────────────────────────────────────────────────────────── */

static void mux_set_channel(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

/* ── ADC ─────────────────────────────────────────────────────────────── */

static void adc_init(void) {
    /* GPIOA + GPIOB + ADC1 clocks */
    EC_RCC_APB2ENR |= (1u << 2) | (1u << 3) | (1u << 9);
    /* ADCCLK = PCLK2/6 = 12 MHz */
    EC_RCC_CFGR = (EC_RCC_CFGR & ~(3u << 14)) | (2u << 14);

    /* Power on ADC */
    EC_ADC1_CR2 = 1u << 0;
    wait_ms(2);

    /* Reset calibration */
    EC_ADC1_CR2 |= (1u << 3);
    { volatile uint32_t t = 50000; while ((EC_ADC1_CR2 & (1u << 3)) && --t); }

    /* Calibration */
    EC_ADC1_CR2 |= (1u << 2);
    { volatile uint32_t t = 200000; while ((EC_ADC1_CR2 & (1u << 2)) && --t); }

    /* CR1: SCAN mode (bit8) = 1 */
    EC_ADC1_CR1 = (1u << 8);

    /* CR2: ADON | EXTTRIG | EXTSEL=111 (SWSTART), CONT=0 */
    EC_ADC1_CR2 = (1u << 0) | (1u << 20) | (7u << 17);

    /* Sample time 239.5 cycles for ch0-9 (slowest, most accurate) */
    EC_ADC1_SMPR2 = 0xFFFFFFFFu;

    /* 6 conversions in sequence */
    EC_ADC1_SQR1 = (5u << 20);
    EC_ADC1_SQR3 = (2u)        /* rank1 IN2 (row0 PA2) */
                 | (3u << 5)   /* rank2 IN3 (row1 PA3) */
                 | (4u << 10)  /* rank3 IN4 (row2 PA4) */
                 | (5u << 15)  /* rank4 IN5 (row3 PA5) */
                 | (6u << 20)  /* rank5 IN6 (row4 PA6) */
                 | (7u << 25); /* rank6 IN7 (row5 PA7) */
}

static void adc_scan_rows(uint16_t *out) {
    EC_ADC1_CR2 |= (1u << 22);  /* SWSTART */
    for (int i = 0; i < 6; i++) {
        volatile uint32_t t = ADC_TIMEOUT;
        while (!(EC_ADC1_SR & (1u << 1)) && --t);
        out[i] = (uint16_t)(EC_ADC1_DR & 0xFFFu);
    }
}

/* ── Read one column ─────────────────────────────────────────────────── */

static void read_column(uint8_t col, uint16_t *vals) {
    mux_set_channel(col);

    pa1_output_low();
    rows_discharge();
    wait_us(DISCHARGE_US);

    rows_sense();
    pa1_output_high();
    wait_us(SETTLE_US);

    adc_scan_rows(vals);

    pa1_output_low();
    rows_discharge();
}

/* ── Baseline ────────────────────────────────────────────────────────── */

static void calibrate_baseline(void) {
    gpio_write_pin(MUX_EN_PIN, 0);
    wait_us(100);

    uint16_t vals[6];
    for (uint8_t col = 0; col < COL_COUNT; col++) {
        uint32_t sums[6] = {0};
        for (int s = 0; s < BASELINE_SAMPLES; s++) {
            read_column(col, vals);
            for (int r = 0; r < 6; r++) sums[r] += vals[r];
        }
        for (int r = 0; r < 6; r++)
            baseline[r][col] = (uint16_t)(sums[r] / BASELINE_SAMPLES);
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

    xprintf("EC87v31 raw %u-%u @R%uC%u | Dmax=%d @R%uC%u\n",
            min_raw, max_raw, mr_r, mr_c, max_diff, md_r, md_c);

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

    print("\nEC87 v3.1 booting...\n");

    EC_RCC_APB2ENR |= (1u << 2) | (1u << 3);  /* GPIOA + GPIOB */

    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 1);

    print("ADC init...\n");
    adc_init();
    print("Baseline calibration...\n");
    calibrate_baseline();
    print("EC87 v3.1 READY - drive col PA1, sense rows PA2-7 via ADC\n\n");
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;
    uint16_t vals[6];

    gpio_write_pin(MUX_EN_PIN, 0);

    for (uint8_t row = 0; row < ROW_COUNT; row++)
        current_matrix[row] = 0;

    for (uint8_t col = 0; col < COL_COUNT; col++) {
        read_column(col, vals);

        for (uint8_t row = 0; row < ROW_COUNT; row++) {
            uint16_t v = vals[row];
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
