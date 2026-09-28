// Copyright 2024 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_numpad_5x5(
        KC_DEL,  KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_HOME, LT(7,KC_P7), KC_P8, KC_P9,
        KC_PGUP, LT(4,KC_P4), LT(5,KC_P5), LT(6,KC_P6), KC_PPLS,
        KC_PGDN, LT(1,KC_P1), LT(2,KC_P2), LT(3,KC_P3),
        MO(1),   KC_P0,   KC_PDOT, KC_PENT
    ),

    [1] = LAYOUT_numpad_5x5(
        _______, _______, _______, MO(2),   KC_CALC,
        _______, KC_HOME, KC_UP,   KC_PGUP,
        _______, KC_LEFT, XXXXXXX, KC_RGHT, _______,
        _______, KC_END,  KC_DOWN, KC_PGDN,
        _______, KC_LSFT,          KC_RSFT, KC_N
    ),

    [2] = LAYOUT_numpad_5x5(
        _______, _______, _______, _______, RM_VALU,
        _______, RM_TOGG, RM_SATU, _______,
        _______, RM_HUED, EE_CLR,  RM_HUEU, RM_VALD,
        _______, _______, RM_SATD, _______,
        _______, RM_NEXT,          _______, _______
    )
};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (host_keyboard_led_state().num_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(1, 255, 255, 255);
    }

    switch (get_highest_layer(layer_state)) {
        case 1: RGB_MATRIX_INDICATOR_SET_COLOR(15, 255, 255, 255); break;
        case 2: RGB_MATRIX_INDICATOR_SET_COLOR(16, 255, 255, 255); break;
        case 3: RGB_MATRIX_INDICATOR_SET_COLOR(17, 255, 255, 255); break;
        case 4: RGB_MATRIX_INDICATOR_SET_COLOR(10, 255, 255, 255); break;
        case 5: RGB_MATRIX_INDICATOR_SET_COLOR(11, 255, 255, 255); break;
        case 6: RGB_MATRIX_INDICATOR_SET_COLOR(12, 255, 255, 255); break;
        case 7: RGB_MATRIX_INDICATOR_SET_COLOR(6, 255, 255, 255); break;
    }
    return false;
}
