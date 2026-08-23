/*
Copyright 2026 micahyy

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "matrix.h"
#include "analog.h"
#include "wait.h"
#include "gpio.h"
#include "debounce.h"
#include "timer.h"

/*
 * EC87 capacitive sensing matrix
 *
 * 74HC4067 16-channel analog mux:
 *   S0=PB8, S1=PB9, S2=PB10, S3=PB11, EN=PB12
 *   COM -> PA1 (ADC1_IN1)
 *
 * 6 row drive pins: PA2-PA7
 * 16 column channels via 74HC4067
 */

#define ROW_COUNT 6
#define COL_COUNT 16

/* Row drive pins PA2-PA7 */
static const pin_t row_pins[ROW_COUNT] = {A2, A3, A4, A5, A6, A7};

/* Mux control: S0=PB8, S1=PB9, S2=PB10, S3=PB11, EN=PB12 */
static const pin_t mux_pins[4] = {B8, B9, B10, B11};
#define MUX_EN_PIN B12

/* ADC input pin (COM from 74HC4067) */
#define ADC_PIN A1

/*
 * Tuning parameters - may need hardware adjustment:
 * DISCHARGE_US: time to discharge before charging
 * CHARGE_US: charging time through row
 * THRESHOLD: ADC difference from baseline to trigger key press
 * SAMPLES_AVG: samples averaged per reading
 * BASELINE_SAMPLES: samples for initial calibration
 */
#define DISCHARGE_US      5
#define CHARGE_US         10
#define THRESHOLD         150
#define SAMPLES_AVG       3
#define BASELINE_SAMPLES  16

static matrix_row_t raw_matrix[ROW_COUNT];
static uint16_t baseline[ROW_COUNT][COL_COUNT];
static bool matrix_ready = false;

static void mux_set_channel(uint8_t ch) {
    for (int i = 0; i < 4; i++) {
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
    }
}

static void mux_enable(void) {
    gpio_write_pin(MUX_EN_PIN, 0);  /* 74HC4067 active low */
}

static void mux_disable(void) {
    gpio_write_pin(MUX_EN_PIN, 1);
}

static uint16_t read_capacitance(uint8_t row, uint8_t col) {
    mux_set_channel(col);

    /* Discharge: drive row low */
    gpio_write_pin(row_pins[row], 0);
    wait_us(DISCHARGE_US);

    /* Charge: drive row high, ADC reads capacitor voltage */
    gpio_write_pin(row_pins[row], 1);
    wait_us(CHARGE_US);

    uint32_t sum = 0;
    for (int i = 0; i < SAMPLES_AVG; i++) {
        sum += analogReadPin(ADC_PIN);
    }

    /* Discharge after reading */
    gpio_write_pin(row_pins[row], 0);
    wait_us(DISCHARGE_US);

    return (uint16_t)(sum / SAMPLES_AVG);
}

static void calibrate_baseline(void) {
    mux_enable();

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint32_t sum = 0;
            for (int s = 0; s < BASELINE_SAMPLES; s++) {
                sum += read_capacitance(row, col);
            }
            baseline[row][col] = (uint16_t)(sum / BASELINE_SAMPLES);
        }
    }

    mux_disable();
}

void matrix_init_custom(void) {
    /* Row drive pins as output push-pull, start low */
    for (int i = 0; i < ROW_COUNT; i++) {
        gpio_set_pin_output(row_pins[i]);
        gpio_write_pin(row_pins[i], 0);
    }

    /* Mux select pins as output push-pull, start low (channel 0) */
    for (int i = 0; i < 4; i++) {
        gpio_set_pin_output(mux_pins[i]);
        gpio_write_pin(mux_pins[i], 0);
    }

    /* Mux enable pin as output, start disabled */
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 1);

    /* ADC pin - analogReadPin handles configuration */
    analogReference(ADC_REF_POWER);  /* VDDA ~3.3V */

    /* Calibrate baseline - do not press keys during boot */
    calibrate_baseline();

    matrix_ready = true;
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    mux_enable();

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        matrix_row_t new_row = 0;

        for (uint8_t col = 0; col < COL_COUNT; col++) {
            uint16_t val = read_capacitance(row, col);
            int16_t diff = (int16_t)val - (int16_t)baseline[row][col];

            /* abs(diff) > threshold -> key pressed (works for either polarity) */
            if (diff > THRESHOLD || diff < -THRESHOLD) {
                new_row |= (matrix_row_t)1 << col;
            }

            /* Slow baseline drift adaptation */
            if (diff > -THRESHOLD && diff < THRESHOLD) {
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

    mux_disable();

    return changed;
}
