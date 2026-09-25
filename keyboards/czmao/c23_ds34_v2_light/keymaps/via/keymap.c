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


// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.

// enum custom_keycodes {
//   QWERTY = SAFE_RANGE,
//   LOWER,
//   RAISE
// };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        //0             1               2               3               4               5               6        7        8        9        10       11       12       13       14       15       16       17       18       19       20       21

        KC_PSCR,        KC_SCRL,        KC_PAUS,        MO(1),          KC_PGUP,        KC_PGDN,        KC_MUTE,    KC_VOLD,    KC_VOLU,

        KC_INS,         KC_HOME,        KC_PGUP,        KC_NUM,         KC_PSLS,        KC_PAST,        KC_PMNS, 
        KC_DEL,         KC_END,         KC_PGDN,        KC_P7,          KC_P8,          KC_P9,          KC_PPLS,
                                                        KC_P4,          KC_P5,          KC_P6,    
                        KC_UP,                          KC_P1,          KC_P2,          KC_P3,          KC_PENT,
        KC_LEFT,        KC_DOWN,        KC_RGHT,        KC_P0,                          KC_PDOT
    ),
    [1] = LAYOUT(
        //1      2        3        4        5        6        7        8        9        10       11       12       13       14       15       16       17       18       19       20       21
        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        TO(3),          KC_TRNS,        KC_TRNS,    KC_TRNS,    KC_TRNS, 

        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS, 
        KC_TRNS,        KC_TRNS,        KC_TRNS,        RGB_TOG,        KC_TRNS,        KC_TRNS,        KC_TRNS,
                                                        KC_TRNS,        RGB_MOD,        KC_TRNS,    
                        KC_TRNS,                        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,
        KC_TRNS,        MO(3),          KC_TRNS,        RGB_MOD,                        KC_TRNS

    ),
    [2] = LAYOUT(
        //1      2        3        4        5        6        7        8        9        10       11       12       13       14       15       16       17       18       19       20       21
        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,    KC_TRNS,     KC_TRNS, 

        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,     
        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,
                                                        KC_TRNS,        KC_TRNS,        KC_TRNS,
                        KC_TRNS,                        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,
        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,                        EE_CLR
    ),
    [3] = LAYOUT(
        //1      2        3        4        5        6        7        8        9        10       11       12       13       14       15       16       17       18       19       20       21
        QK_BOOT,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,    KC_PGUP,     KC_PGDN, 

        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,     
        KC_TRNS,        KC_TRNS,        KC_TRNS,        LCTL(KC_F7),    LCTL(KC_F8),    LCTL(KC_F9),    KC_TRNS,
                                                        LCTL(KC_F4),    LCTL(KC_F5),    LCTL(KC_F6),
                        KC_TRNS,                        LCTL(KC_F1),    LCTL(KC_F2),    LCTL(KC_F3),    LCTL(KC_F12),
        KC_TRNS,        KC_TRNS,        KC_TRNS,        LCTL(KC_F10),                   LCTL(KC_F11)     
    )
    
};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // caps lock cyan

    // num lock cyan
    if (host_keyboard_led_state().num_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(9, 255, 255, 255);
    } 



    for (uint8_t i = led_min; i < led_max; i++) {
        switch(get_highest_layer(layer_state|default_layer_state)) {
            case 4:
                rgb_matrix_set_color(i, RGB_PINK);
                break;
            case 3:
                rgb_matrix_set_color(i, RGB_BLUE);
                break;
            case 2:
                rgb_matrix_set_color(i, RGB_GREEN);
                break;
            case 1:
                rgb_matrix_set_color(i, RGB_RED);
                break;
            default:
                break;
        }
    }

    return false;
}
