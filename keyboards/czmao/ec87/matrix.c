/* Copyright 2026 micahyy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "matrix.h"
#include "gpio.h"
#include "wait.h"
#include "ch.h"
#include "hal.h"
#include "print.h"
#include <stdint.h>

/*
 * EC87 capacitive sensing matrix (CUSTOM_MATRIX = lite) - DEBUG BUILD
 *
 * Hardware:
 *   74HC4067: S0=PB8, S1=PB9, S2=PB10, S3=PB11, /EN=PB12
 *   COM -> PA1 (ADC1_IN1)
 *   6 row drive pins: PA2-PA7
 *
 * Sensing: charge redistribution
 *   Discharge both ends -> switch PA1 to analog -> pulse row HIGH -> read ADC.
 *
 * DEBUG: prints raw ADC + diff for all 96 keys over QMK console.
 *        Use QMK Toolbox or `qmk console` to view.
 */

#define ROW_COUNT 6
#define COL_COUNT 16

static const pin_t row_pins[ROW_COUNT] = {A2, A3, A4, A5, A6, A7};
static const pin_t mux_pins[4]       = {B8, B9, B10, B11};
#define MUX_EN_PIN B12

/* ── Tuning ──────────────────────────────────────────────────────────── */
#define THRESHOLD         60
#define DISCHARGE_US      3
#define BASELINE_SAMPLES  32

/* Debug print interval (in scans). Each full scan ≈ 2-3 ms. */
#define DEBUG_PRINT_INTERVAL  30

/* ── STM32F103 ADC registers ─────────────────────────────────────────── */
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018)
#define ADC1_BASE     0x40012400
#define ADC1_SR       (*(volatile uint32_t *)(ADC1_BASE + 0x00))
#define ADC1_CR1      (*(volatile uint32_t *)(ADC1_BASE + 0x04))
#define ADC1_CR2      (*(volatile uint32_t *)(ADC1_BASE + 0x08))
#define ADC1_SMPR2    (*(volatile uint32_t *)(ADC1_BASE + 0x10))
#define ADC1_SQR3     (*(volatile uint32_t *)(ADC1_BASE + 0x2C))
#define ADC1_SQR1     (*(volatile uint32_t *)(ADC1_BASE + 0x28))
#define ADC1_DR       (*(volatile uint32_t *)(ADC1_BASE + 0x4C))

#define ADC_CR2_ADON       (1u << 0)
#define ADC_CR2_CAL        (1u << 2)
#define ADC_CR2_RSTCAL     (1u << 3)
#define ADC_CR2_EXTTRIG    (1u << 20)
#define ADC_CR2_EXTSEL_POS 17
#define ADC_CR2_SWSTARTBIT (1u << 22)
#define ADC_SR_EOC         (1u << 1)

#define GPIOA_CRL        (*(volatile uint32_t *)0x40010800)
#define GPIOA_BSRR       (*(volatile uint32_t *)0x40010810)
#define GPIOA_PA1_MASK   0x000000F0u

/* ── State ───────────────────────────────────────────────────────────── */
static uint16_t baseline[ROW_COUNT][COL_COUNT];
static uint16_t adc_raw[ROW_COUNT][COL_COUNT];   /* last raw reading */
static int16_t  adc_diff[ROW_COUNT][COL_COUNT];  /* last diff */
static uint8_t  scan_counter = 0;

/* ── PA1 pin mode ────────────────────────────────────────────────────── */

static inline void pa1_set_analog(void) {
    GPIOA_CRL = (GPIOA_CRL & ~GPIOA_PA1_MASK);
}

static inline void pa1_discharge_low(void) {
    GPIOA_CRL = (GPIOA_CRL & ~GPIOA_PA1_MASK) | (0x3u << 4);
    GPIOA_BSRR = (1u << 17);
}

/* ── Mux ─────────────────────────────────────────────────────────────── */

static void mux_set_channel(uint8_t ch) {
    for (int i = 0; i < 4; i++) {
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
    }
}

static void mux_enable(void)  { gpio_write_pin(MUX_EN_PIN, 0); }
static void mux_disable(void) { gpio_write_pin(MUX_EN_PIN, 1); }

/* ── ADC ─────────────────────────────────────────────────────────────── */

static void adc_init(void) {
    RCC_APB2ENR |= (1u << 2) | (1u << 9);  /* IOPAEN + ADC1EN */

    /* PCLK2/6 = 12 MHz */
    volatile uint32_t *rcc_cfgr = (volatile uint32_t *)0x40021004;
    *rcc_cfgr = (*rcc_cfgr & ~(3u << 14)) | (2u << 14);

    ADC1_CR2 = ADC_CR2_ADON;
    wait_ms(1);

    ADC1_CR2 |= ADC_CR2_RSTCAL;
    while (ADC1_CR2 & ADC_CR2_RSTCAL) { ; }

    ADC1_CR2 |= ADC_CR2_CAL;
    while (ADC1_CR2 & ADC_CR2_CAL) { ; }

    ADC1_CR2 = ADC_CR2_ADON | ADC_CR2_EXTTRIG | (7u << ADC_CR2_EXTSEL_POS);
    ADC1_CR1 = 0;
    ADC1_SQR1 = 0;
    ADC1_SMPR2 = (ADC1_SMPR2 & ~(7u << 3)) | (7u << 3);  /* 239.5 cycles */
    ADC1_SQR3 = (ADC1_SQR3 & ~0x1Fu) | 1u;  /* channel 1 */
}

static uint16_t adc_read_once(void) {
    ADC1_CR2 |= ADC_CR2_SWSTARTBIT;
    uint32_t timeout = 10000;
    while (!(ADC1_SR & ADC_SR_EOC) && --timeout) { ; }
    return (uint16_t)(ADC1_DR & 0xFFFu);
}

/* ── Capacitance read ────────────────────────────────────────────────── */

static uint16_t read_capacitance(uint8_t row, uint8_t col) {
    mux_set_channel(col);

    pa1_discharge_low();
    gpio_write_pin(row_pins[row], 0);
    wait_us(DISCHARGE_US);

    pa1_set_analog();
    gpio_write_pin(row_pins[row], 1);

    uint16_t val = adc_read_once();

    gpio_write_pin(row_pins[row], 0);
    return val;
}

/* ── Baseline calibration ────────────────────────────────────────────── */

static void calibrate_baseline(void) {
    mux_enable();
    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        for (uint8_t r = 0; r < ROW_COUNT; r++) {
            if (r == row) { gpio_set_pin_output(row_pins[r]); gpio_write_pin(row_pins[r], 0); }
            else          { gpio_set_pin_input(row_pins[r]); }
        }
        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint32_t sum = 0;
            for (int s = 0; s < BASELINE_SAMPLES; s++)
                sum += read_capacitance(row, col);
            baseline[row][col] = (uint16_t)(sum / BASELINE_SAMPLES);
        }
    }
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        gpio_set_pin_output(row_pins[r]);
        gpio_write_pin(row_pins[r], 0);
    }
    mux_disable();
}

/* ── Debug print ─────────────────────────────────────────────────────── */

static void debug_print(void) {
    /* Find max diff across the matrix */
    int16_t max_diff = 0;
    uint8_t max_row = 0, max_col = 0;
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        for (uint8_t c = 0; c < COL_COUNT; c++) {
            if (adc_diff[r][c] > max_diff) {
                max_diff = adc_diff[r][c];
                max_row = r;
                max_col = c;
            }
        }
    }

    print("=== EC87 ADC === max_diff=");
    print("R");
    xprintf("%d", max_row);
    print("C");
    xprintf("%d", max_col);
    print(" val=");
    xprintf("%d", adc_raw[max_row][max_col]);
    print(" base=");
    xprintf("%d", baseline[max_row][max_col]);
    print(" diff=");
    xprintf("%d", max_diff);
    print("\n");

    /*
     * Print a compact 6x16 grid of diff values.
     * Format: R#: c00 c01 c02 ... c15
     * Values are signed, fixed-width 4 chars.
     */
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%d:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++) {
            xprintf(" %4d", adc_diff[r][c]);
        }
        print("\n");
    }
    print("\n");
}

/* ── QMK interface ───────────────────────────────────────────────────── */

void matrix_init_custom(void) {
    /* Init debug console */
    debug_enable = true;
    debug_matrix = true;

    for (int i = 0; i < ROW_COUNT; i++) {
        gpio_set_pin_output(row_pins[i]);
        gpio_write_pin(row_pins[i], 0);
    }
    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 1);

    adc_init();
    calibrate_baseline();

    print("\n\n=== EC87 DEBUG BUILD === ADC init OK, baseline calibrated\n");
    print("Press keys — watch for positive diff spikes.\n");
    print("If nothing changes: check 74HC4067 EN/wiring, ADC pin, row drive.\n\n");
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    mux_enable();

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        matrix_row_t new_row = 0;

        for (uint8_t r = 0; r < ROW_COUNT; r++) {
            if (r == row) { gpio_set_pin_output(row_pins[r]); gpio_write_pin(row_pins[r], 0); }
            else          { gpio_set_pin_input(row_pins[r]); }
        }

        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint16_t val = read_capacitance(row, col);
            int16_t  diff = (int16_t)val - (int16_t)baseline[row][col];

            adc_raw[row][col]  = val;
            adc_diff[row][col] = diff;

            if (diff > THRESHOLD) {
                new_row |= (matrix_row_t)1 << col;
            }
            if (diff < THRESHOLD) {
                if (val > baseline[row][col]) baseline[row][col]++;
                else if (val < baseline[row][col]) baseline[row][col]--;
            }
        }

        if (new_row != current_matrix[row]) {
            current_matrix[row] = new_row;
            changed = true;
        }
    }

    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        gpio_set_pin_output(row_pins[r]);
        gpio_write_pin(row_pins[r], 0);
    }
    mux_disable();

    /* Periodic debug output */
    scan_counter++;
    if (scan_counter >= DEBUG_PRINT_INTERVAL) {
        scan_counter = 0;
        debug_print();
    }

    return changed;
}
