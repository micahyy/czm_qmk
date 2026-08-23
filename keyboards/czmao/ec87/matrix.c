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
#include <stdint.h>

/*
 * EC87 capacitive sensing matrix (CUSTOM_MATRIX = lite)
 *
 * Hardware:
 *   74HC4067 16-channel analog mux: S0=PB8, S1=PB9, S2=PB10, S3=PB11, /EN=PB12
 *   COM -> PA1 (ADC1_IN1)
 *   6 row drive pins: PA2-PA7
 *
 * Sensing method: charge redistribution (capacitive coupling)
 *   The key acts as a capacitor between row and column (mux input).
 *   1. Discharge column line (PA1 output LOW) and target row LOW
 *   2. Switch PA1 to analog input (high-Z)
 *   3. Drive target row HIGH -> rising edge couples through key cap onto column
 *   4. ADC reads the coupled voltage: V = 3.3 * C_key / (C_key + C_parasitic)
 *   Pressed key => larger C_key => higher ADC reading.
 */

#define ROW_COUNT 6
#define COL_COUNT 16

/* Row drive pins PA2-PA7 */
static const pin_t row_pins[ROW_COUNT] = {A2, A3, A4, A5, A6, A7};

/* Mux select pins: S0=PB8, S1=PB9, S2=PB10, S3=PB11 */
static const pin_t mux_pins[4] = {B8, B9, B10, B11};
#define MUX_EN_PIN B12

/* ADC pin = PA1 = ADC1_IN1 */
#define ADC_PIN A1

/*
 * Tuning parameters.
 * THRESHOLD: ADC delta from baseline to register a key press.
 *   Charge redistribution gives a large signal — 100 is conservative.
 * DISCHARGE_US: how long to hold both ends at 0V before sampling.
 */
#define THRESHOLD         80
#define DISCHARGE_US      3
#define BASELINE_SAMPLES  32

/* ── STM32F103 ADC register definitions ──────────────────────────────── */
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018)
#define ADC1_BASE     0x40012400
#define ADC1_SR       (*(volatile uint32_t *)(ADC1_BASE + 0x00))
#define ADC1_CR1      (*(volatile uint32_t *)(ADC1_BASE + 0x04))
#define ADC1_CR2      (*(volatile uint32_t *)(ADC1_BASE + 0x08))
#define ADC1_SMPR2    (*(volatile uint32_t *)(ADC1_BASE + 0x10))
#define ADC1_SQR3     (*(volatile uint32_t *)(ADC1_BASE + 0x2C))
#define ADC1_SQR1     (*(volatile uint32_t *)(ADC1_BASE + 0x28))
#define ADC1_DR       (*(volatile uint32_t *)(ADC1_BASE + 0x4C))

/* ADC bit masks */
#define ADC_CR2_ADON       (1u << 0)
#define ADC_CR2_CAL        (1u << 2)
#define ADC_CR2_RSTCAL     (1u << 3)
#define ADC_CR2_EXTTRIG    (1u << 20)
#define ADC_CR2_EXTSEL_POS 17
#define ADC_CR2_SWSTARTBIT (1u << 22)
#define ADC_SR_EOC         (1u << 1)

/* GPIOA CRL for PA1 (bits 7-4): CNF=00 (analog), MODE=00 (input) */
#define GPIOA_CRL        (*(volatile uint32_t *)0x40010800)
#define GPIOA_BSRR       (*(volatile uint32_t *)0x40010810)
#define GPIOA_PA1_MASK   0x000000F0u  /* bits 7:4 for PA1 */

static uint16_t baseline[ROW_COUNT][COL_COUNT];

/* ── PA1 pin mode helpers ────────────────────────────────────────────── */

/* Set PA1 to analog input mode (CNF=00, MODE=00) for ADC reading */
static inline void pa1_set_analog(void) {
    GPIOA_CRL = (GPIOA_CRL & ~GPIOA_PA1_MASK);  /* 0000 = analog input */
}

/* Set PA1 to push-pull output 50MHz and drive LOW for discharge */
static inline void pa1_discharge_low(void) {
    /* CNF=00 (push-pull), MODE=11 (50 MHz) => 0x3, shifted to bits 7:4 */
    GPIOA_CRL = (GPIOA_CRL & ~GPIOA_PA1_MASK) | (0x3u << 4);
    /* Drive PA1 low */
    GPIOA_BSRR = (1u << 17);  /* BR1: reset PA1 */
}

/* ── Mux control ─────────────────────────────────────────────────────── */

static void mux_set_channel(uint8_t ch) {
    for (int i = 0; i < 4; i++) {
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
    }
}

static void mux_enable(void) {
    gpio_write_pin(MUX_EN_PIN, 0);  /* 74HC4067 /E is active low */
}

static void mux_disable(void) {
    gpio_write_pin(MUX_EN_PIN, 1);
}

/* ── ADC initialisation ──────────────────────────────────────────────── */

static void adc_init(void) {
    /* Enable GPIOA and ADC1 clocks */
    RCC_APB2ENR |= (1u << 2)  /* IOPAEN  */
                 |  (1u << 9); /* ADC1EN  */

    /* ADC prescaler PCLK2/6 = 72/6 = 12 MHz (must be <=14 MHz) */
    /* RCC_CFGR ADCPRE bits 15:14 = 10 => PCLK2/6 */
    volatile uint32_t *rcc_cfgr = (volatile uint32_t *)0x40021004;
    *rcc_cfgr = (*rcc_cfgr & ~(3u << 14)) | (2u << 14);

    /* Power on ADC and wait for stabilisation */
    ADC1_CR2 = ADC_CR2_ADON;
    wait_ms(1);

    /* Reset calibration */
    ADC1_CR2 |= ADC_CR2_RSTCAL;
    while (ADC1_CR2 & ADC_CR2_RSTCAL) { ; }

    /* Run calibration */
    ADC1_CR2 |= ADC_CR2_CAL;
    while (ADC1_CR2 & ADC_CR2_CAL) { ; }

    /*
     * Configure CR2:
     *   ADON     = 1   (ADC powered on)
     *   EXTTRIG  = 1   (external trigger conversion enabled)
     *   EXTSEL   = 111 (SWSTART as trigger source)
     *   ALIGN    = 0   (right-aligned 12-bit)
     */
    ADC1_CR2 = ADC_CR2_ADON | ADC_CR2_EXTTRIG | (7u << ADC_CR2_EXTSEL_POS);

    /* CR1: single conversion mode, no analog watchdog, no discontinuous */
    ADC1_CR1 = 0;

    /* SQR1: 1 conversion per sequence (L = 0 means 1 conversion) */
    ADC1_SQR1 = 0;

    /*
     * Sample time for channel 1: 239.5 cycles (slowest, most accurate).
     * SMPR2 bits 5:3 = SMP1, value 111 = 239.5 cycles.
     * At 12 MHz this is ~20 µs sample + conversion time.
     */
    ADC1_SMPR2 = (ADC1_SMPR2 & ~(7u << 3)) | (7u << 3);

    /* SQR3: first (and only) conversion = channel 1 */
    ADC1_SQR3 = (ADC1_SQR3 & ~0x1Fu) | 1u;
}

static uint16_t adc_read_once(void) {
    /* Start conversion by setting SWSTART bit */
    ADC1_CR2 |= ADC_CR2_SWSTARTBIT;

    /* Wait for end of conversion */
    uint32_t timeout = 10000;
    while (!(ADC1_SR & ADC_SR_EOC) && --timeout) { ; }

    return (uint16_t)(ADC1_DR & 0xFFFu);
}

/* ── Capacitance measurement ─────────────────────────────────────────── */

static uint16_t read_capacitance(uint8_t row, uint8_t col) {
    mux_set_channel(col);

    /* Step 1: Discharge — PA1 push-pull LOW, target row LOW */
    pa1_discharge_low();
    gpio_write_pin(row_pins[row], 0);
    wait_us(DISCHARGE_US);

    /* Step 2: Switch PA1 to analog input (high-Z) */
    pa1_set_analog();

    /* Step 3: Drive row HIGH — rising edge couples through key capacitor */
    gpio_write_pin(row_pins[row], 1);

    /* Step 4: Read ADC — the coupled voltage is proportional to C_key */
    uint16_t val = adc_read_once();

    /* Restore row LOW to leave it in a known state */
    gpio_write_pin(row_pins[row], 0);

    return val;
}

/* ── Baseline calibration ────────────────────────────────────────────── */

static void calibrate_baseline(void) {
    mux_enable();

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        /* Active row output; other rows float (same as scan) */
        for (uint8_t r = 0; r < ROW_COUNT; r++) {
            if (r == row) {
                gpio_set_pin_output(row_pins[r]);
                gpio_write_pin(row_pins[r], 0);
            } else {
                gpio_set_pin_input(row_pins[r]);
            }
        }

        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint32_t sum = 0;
            for (int s = 0; s < BASELINE_SAMPLES; s++) {
                sum += read_capacitance(row, col);
            }
            baseline[row][col] = (uint16_t)(sum / BASELINE_SAMPLES);
        }
    }

    /* Restore all rows to output low */
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        gpio_set_pin_output(row_pins[r]);
        gpio_write_pin(row_pins[r], 0);
    }

    mux_disable();
}

/* ── QMK custom matrix interface ─────────────────────────────────────── */

void matrix_init_custom(void) {
    /* Initialise all row pins as output low */
    for (int i = 0; i < ROW_COUNT; i++) {
        gpio_set_pin_output(row_pins[i]);
        gpio_write_pin(row_pins[i], 0);
    }

    /* Mux select pins as output push-pull, start at channel 0 */
    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }

    /* Mux enable: output, start disabled (high) */
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 1);

    /* Initialise ADC1 for PA1 */
    adc_init();

    /* Calibrate — do not press keys during power-up! */
    calibrate_baseline();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    mux_enable();

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        matrix_row_t new_row = 0;

        /* Active row is output; other rows float to avoid extra capacitance */
        for (uint8_t r = 0; r < ROW_COUNT; r++) {
            if (r == row) {
                gpio_set_pin_output(row_pins[r]);
                gpio_write_pin(row_pins[r], 0);
            } else {
                gpio_set_pin_input(row_pins[r]);
            }
        }

        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint16_t val = read_capacitance(row, col);
            int16_t  diff = (int16_t)val - (int16_t)baseline[row][col];

            /* Pressed key => higher capacitance => higher ADC reading */
            if (diff > THRESHOLD) {
                new_row |= (matrix_row_t)1 << col;
            }

            /* Slow baseline drift when key is not pressed */
            if (diff < THRESHOLD) {
                if (val > baseline[row][col]) {
                    baseline[row][col]++;
                } else if (val < baseline[row][col]) {
                    baseline[row][col]--;
                }
            }
        }

        if (new_row != current_matrix[row]) {
            current_matrix[row] = new_row;
            changed = true;
        }
    }

    /* Restore all rows to output low */
    for (uint8_t r = 0; r < ROW_COUNT; r++) {
        gpio_set_pin_output(row_pins[r]);
        gpio_write_pin(row_pins[r], 0);
    }

    mux_disable();

    return changed;
}
