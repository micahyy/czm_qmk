// Copyright 2024 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "wait.h"

void keyboard_pre_init_user(void) {
    // B13: set push-pull output, drive low, wait 300us for WS2812 reset
    palSetLineMode(B13, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(B13);
    wait_us(300);
}

void matrix_init_user(void) {
    rgb_matrix_set_color_all(0, 0, 0);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_numpad_5x5(
        KC_HOME, LT(1,KC_NUM), KC_PSLS, KC_PAST, KC_PMNS,
        KC_END,  KC_P7,  KC_P8,  KC_P9,
        KC_PGUP, KC_P4,  KC_P5,  KC_P6,  KC_PPLS,
        KC_PGDN, KC_P1,  KC_P2,  KC_P3,
        MO(1),   KC_P0,          KC_PDOT, KC_PENT
    ),

    [1] = LAYOUT_numpad_5x5(
        _______, _______, _______, MO(2),   KC_CALC,
        _______, KC_HOME, KC_UP,   KC_PGUP,
        _______, KC_LEFT, XXXXXXX, KC_RGHT, _______,
        _______, KC_END,  KC_DOWN, KC_PGDN,
        _______, _______,          _______, _______
    ),

    [2] = LAYOUT_numpad_5x5(
        _______, _______, _______, _______, RM_VALU,
        _______, RM_TOGG, RM_SATU, _______,
        _______, RM_HUED, EE_CLR,  RM_HUEU, RM_VALD,
        _______, _______, RM_SATD, _______,
        _______, RM_NEXT,          _______, NK_TOGG
    )
};
