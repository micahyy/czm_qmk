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
#include "print.h"
#include "ch.h"
#include "hal.h"
#include <stdint.h>

/*
 * EC87 capacitive sensing matrix (CUSTOM_MATRIX = lite) - DEBUG BUILD
 *
 * Hardware:
 *   74HC4067: S0=PB8, S1=PB9, S2=PB10, S3=PB11, /EN=PB12
 *   COM -> PA1 (ADC1_IN1)
 *   6 row drive pins: PA2-PA7
 *
 * Sensing method: charge transfer (redistribution)
 *   1. PA1 drives HIGH to pre-charge the column/sense plate via mux
 *   2. Row pin is driven LOW (grounded) during pre-charge
 *   3. PA1 switches to analog (high-Z), row switches to floating input (high-Z)
 *   4. Charge redistributes instantly through mux Ron
 *   5. ADC reads PA1: unpressed ~VDD (high), pressed ~lower (charge shared)
 *
 * DEBUG: prints raw ADC + diff for all 96 keys over QMK console.
 */

#define ROW_COUNT 6
#define COL_COUNT 16

static const pin_t row_pins[ROW_COUNT] = {A2, A3, A4, A5, A6, A7};
static const pin_t mux_pins[4]         = {B8, B9, B10, B11};
#define MUX_EN_PIN B12

/* ── Tuning ──────────────────────────────────────────────────────────── */
#define THRESHOLD         80
#define PRECHARGE_US      5
#define BASELINE_SAMPLES  32
#define DEBUG_PRINT_INTERVAL  20

/* ── STM32F103 register helpers ──────────────────────────────────────── */
#define EC_RCC_APB2ENR   (*(volatile uint32_t *)0x40021018)
#define EC_RCC_CFGR      (*(volatile uint32_t *)0x40021004)

#define EC_ADC1_SR       (*(volatile uint32_t *)0x40012400)
#define EC_ADC1_CR1      (*(volatile uint32_t *)0x40012404)
#define EC_ADC1_CR2      (*(volatile uint32_t *)0x40012408)
#define EC_ADC1_SMPR2    (*(volatile uint32_t *)0x40012410)
#define EC_ADC1_SQR1     (*(volatile uint32_t *)0x40012428)
#define EC_ADC1_SQR3     (*(volatile uint32_t *)0x4001242C)
#define EC_ADC1_DR       (*(volatile uint32_t *)0x4001244C)

#define EC_GPIOA_CRL     (*(volatile uint32_t *)0x40010800)
#define EC_GPIOA_BSRR    (*(volatile uint32_t *)0x40010810)
#define EC_GPIOA_IDR     (*(volatile uint32_t *)0x40010808)

/* PA1 bits in CRL: [7:4] */
#define EC_PA1_MASK      0x000000F0u
/* PA2-PA7 bits in CRL: [31:8] */
#define EC_ROW_MASK      0xFFFFFF00u  /* covers PA2..PA7 (bits 8-31) */

/* ── State ───────────────────────────────────────────────────────────── */
static uint16_t baseline[ROW_COUNT][COL_COUNT];
static uint16_t adc_raw[ROW_COUNT][COL_COUNT];
static int16_t  adc_diff[ROW_COUNT][COL_COUNT];
static uint8_t scan_counter = 0;

/* ── PA1 pin mode helpers ────────────────────────────────────────────── */

/* PA1: analog input (high-Z) */
static inline void pa1_analog(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK);  /* CNF=00, MODE=00 */
}

/* PA1: push-pull output 50MHz, drive HIGH */
static inline void pa1_drive_high(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 1);   /* BS1 */
}

/* PA1: push-pull output 50MHz, drive LOW */
static inline void pa1_drive_low(void) {
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_PA1_MASK) | (0x3u << 4);
    EC_GPIOA_BSRR = (1u << 17);  /* BR1 */
}

/* ── Row pin mode (direct register, PA2-PA7 = CRL bits 8-31) ─────────── */

/* Set all 6 rows to floating input (high-Z) */
static inline void rows_all_floating(void) {
    /* CNF=01 (floating input), MODE=00 (input) for PA2..PA7
     * Each pin uses 4 bits: CNF[1:0] MODE[1:0]
     * Floating input = 0x4 (CNF=01, MODE=00)
     * PA2 starts at bit 8. Set all 6 pins to 0x4 in their nibble. */
    EC_GPIOA_CRL = (EC_GPIOA_CRL & ~EC_ROW_MASK) | (0x44444400u);
}

/* Set specific row to push-pull output LOW, others floating */
static inline void row_set_output_low(uint8_t row) {
    rows_all_floating();
    /* For the active row pin (PA2+row), set CNF=00, MODE=11 (50MHz PP output)
     * Pin offset = 8 + row*4. Output mode = 0x3. */
    uint8_t shift = 8 + row * 4;
    EC_GPIOA_CRL &= ~(0xFu << shift);
    EC_GPIOA_CRL |=  (0x3u << shift);
    /* Drive LOW: BR bit = 16 + (2+row) = 18+row */
    EC_GPIOA_BSRR = (1u << (18 + row));
}

/* ── Mux ─────────────────────────────────────────────────────────────── */

static void mux_set_channel(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

static void mux_enable(void)  { gpio_write_pin(MUX_EN_PIN, 0); }
static void mux_disable(void) { gpio_write_pin(MUX_EN_PIN, 1); }

/* ── ADC ─────────────────────────────────────────────────────────────── */

static void adc_init(void) {
    EC_RCC_APB2ENR |= (1u << 2) | (1u << 9);   /* IOPAEN + ADC1EN */

    /* ADC clock: PCLK2 / 6 = 12 MHz */
    EC_RCC_CFGR = (EC_RCC_CFGR & ~(3u << 14)) | (2u << 14);

    EC_ADC1_CR2 = 1u << 0;                     /* ADON */
    wait_ms(1);

    EC_ADC1_CR2 |= (1u << 3);                  /* RSTCAL */
    while (EC_ADC1_CR2 & (1u << 3)) { ; }

    EC_ADC1_CR2 |= (1u << 2);                  /* CAL */
    while (EC_ADC1_CR2 & (1u << 2)) { ; }

    EC_ADC1_CR2 = (1u << 0) | (1u << 20) | (7u << 17);  /* ADON|EXTTRIG|SWSTART */
    EC_ADC1_CR1 = 0;
    EC_ADC1_SQR1 = 0;

    /* Sample time: 55.5 cycles (~4.6µs) for stable reading after redistribution */
    EC_ADC1_SMPR2 = (EC_ADC1_SMPR2 & ~(7u << 3)) | (5u << 3);  /* SMP1=101 */
    EC_ADC1_SQR3 = (EC_ADC1_SQR3 & ~0x1Fu) | 1u;
}

static uint16_t adc_read_once(void) {
    EC_ADC1_CR2 |= (1u << 22);                 /* SWSTART */
    uint32_t t = 10000;
    while (!(EC_ADC1_SR & (1u << 1)) && --t) { ; }
    return (uint16_t)(EC_ADC1_DR & 0xFFFu);
}

/* ── Capacitance read (charge transfer) ──────────────────────────────── */

static uint16_t read_capacitance(uint8_t row, uint8_t col) {
    mux_set_channel(col);

    /* Step 1: Pre-charge. PA1=PP HIGH, active row=PP LOW, others floating. */
    row_set_output_low(row);
    pa1_drive_high();
    wait_us(PRECHARGE_US);

    /* Step 2: Float everything simultaneously.
     * PA1 -> analog (high-Z), active row -> floating input.
     * Charge redistributes through mux Ron (~100Ω, RC ~1-2ns). */
    pa1_analog();
    rows_all_floating();

    /* Step 3: Read ADC.
     * Unpressed: COM held near VDD by column parasitic cap -> high reading
     * Pressed: charge shared with key cap + row parasitic -> lower reading */
    uint16_t val = adc_read_once();
    return val;
}

/* ── Baseline calibration ────────────────────────────────────────────── */

static void calibrate_baseline(void) {
    mux_enable();
    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint32_t sum = 0;
            for (int s = 0; s < BASELINE_SAMPLES; s++)
                sum += read_capacitance(row, col);
            baseline[row][col] = (uint16_t)(sum / BASELINE_SAMPLES);
        }
    }
    rows_all_floating();
    mux_disable();
}

/* ── Debug print ─────────────────────────────────────────────────────── */

static void debug_print(void) {
    uint16_t max_raw = 0, min_raw = 4095;
    int16_t  max_diff = -4096, min_diff = 4096;
    uint8_t  mr_r = 0, mr_c = 0, md_r = 0, md_c = 0;

    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        for (uint8_t c = 0; c < COL_COUNT; c++) {
            uint16_t v = adc_raw[r][c];
            int16_t  d = adc_diff[r][c];
            if (v > max_raw) { max_raw = v; mr_r = r; mr_c = c; }
            if (v < min_raw)   min_raw = v;
            if (d > max_diff) { max_diff = d; md_r = r; md_c = c; }
            if (d < min_diff)   min_diff = d;
        }
    }

    xprintf("=== EC87 | raw %u-%u @R%uC%u | diff %d..%d @R%uC%u ===\n",
            min_raw, max_raw, mr_r, mr_c, min_diff, max_diff, md_r, md_c);

    print("RAW:\n");
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%u:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++)
            xprintf(" %4u", adc_raw[r][c]);
        print("\n");
    }

    print("DIFF:\n");
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        xprintf("R%u:", r);
        for (uint8_t c = 0; c < COL_COUNT; c++)
            xprintf(" %4d", adc_diff[r][c]);
        print("\n");
    }
    print("\n");
}

/* ── QMK interface ───────────────────────────────────────────────────── */

void matrix_init_custom(void) {
    /* Mux pins as outputs */
    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 1);  /* disabled initially */

    adc_init();
    calibrate_baseline();

    print("\n\n=== EC87 CHARGE-TRANSFER DEBUG READY ===\n");
    print("PA1 pre-charge HIGH -> float both -> ADC read.\n");
    print("Pressed key should show LOWER raw value (negative diff).\n\n");
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    mux_enable();

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        matrix_row_t new_row = 0;

        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint16_t val = read_capacitance(row, col);
            int16_t  diff = (int16_t)baseline[row][col] - (int16_t)val;  /* pressed => positive diff */

            adc_raw[row][col]  = val;
            adc_diff[row][col] = diff;

            if (diff > THRESHOLD)
                new_row |= (matrix_row_t)1 << col;

            /* Slow baseline tracking */
            if (diff < THRESHOLD) {
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
