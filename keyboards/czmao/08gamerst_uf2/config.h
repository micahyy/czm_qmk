/* Copyright 2025 MAOKB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define DYNAMIC_KEYMAP_LAYER_COUNT 8

// WS2812 data pin B13 on STM32F103 maps to TIM1_CH1N (complementary output,
// full-remap). Use the QMK pwm driver with PWMD1 / CH1 / complementary and
// DMA1 Stream5 (TIM1_UP on F103). Kept in sync with the working reference in
// qmk_czmao/keyboards/czmao/ds22/ds22_r6_uf2/config.h.
#define WS2812_PWM_DRIVER               PWMD1
#define WS2812_PWM_CHANNEL              1
#define WS2812_PWM_COMPLEMENTARY_OUTPUT
#define WS2812_PWM_DMA_STREAM           STM32_DMA1_STREAM5
#define WS2812_PWM_DMA_CHANNEL          5

#ifdef ENCODER_ENABLE
// Pins and per-encoder resolution now come from keyboard.json -> ENCODER_A_PINS /
// ENCODER_B_PINS / ENCODER_RESOLUTIONS. Do not redefine them here, the JSON based
// preprocessor generates them and duplicate definitions would clash.

// keypos_t is {col, row}. Row 5 columns 4/5 are the rotary-only positions.
#    define ENCODERS_MATRIX_MAP {{{4, 5}, {5, 5}}}
#endif
