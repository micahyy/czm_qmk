/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include QMK_KEYBOARD_H
#include "wait.h"

void keyboard_pre_init_user(void) {
    palSetLineMode(B13, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(B13);
    wait_us(300);
}

void matrix_init_user(void) {
    rgb_matrix_set_color_all(0, 0, 0);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,
        KC_LCTL, MO(1),   KC_LALT, KC_SPC
    ),

    [1] = LAYOUT(
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, MO(2),   _______
    ),
    [2] = LAYOUT(
        EE_CLR,  _______, _______, _______, _______, QK_BOOT,
        _______, _______, _______, _______, _______, _______, _______,
        NK_TOGG, RM_VALU, RM_HUEU, RM_SATU, _______, _______,
        _______, _______, RM_HUED, RM_SATD, RM_VALD, RM_SPDD,
        RM_TOGG, RM_NEXT, RM_HUED, RM_SATD, RM_VALD, RM_SPDU,
        _______, _______, _______, _______
    )
};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (host_keyboard_led_state().caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(19, 255, 255, 255);
    }

    switch (get_highest_layer(layer_state)) {
        case 1: RGB_MATRIX_INDICATOR_SET_COLOR(32, 255, 255, 255); break;
        case 2: RGB_MATRIX_INDICATOR_SET_COLOR(7, 255, 255, 255); break;
        case 3: RGB_MATRIX_INDICATOR_SET_COLOR(8, 255, 255, 255); break;
        case 4: RGB_MATRIX_INDICATOR_SET_COLOR(9, 255, 255, 255); break;
        case 5: RGB_MATRIX_INDICATOR_SET_COLOR(10, 255, 255, 255); break;
        case 6: RGB_MATRIX_INDICATOR_SET_COLOR(11, 255, 255, 255); break;
        case 7: RGB_MATRIX_INDICATOR_SET_COLOR(12, 255, 255, 255); break;
        default: break;
    }
    return false;
}
