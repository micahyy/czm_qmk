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
        //1         2           3           4           5           6           7        8        9        10       11       12       13       14       15       16       17       18       19       20       21
        KC_ESC,     KC_F1,      KC_F2,      KC_F3,      KC_F4,      KC_MUTE,
        KC_GRV,     KC_1,       KC_2,       KC_3,       KC_4,       KC_5,       KC_6,
        KC_TAB,     KC_Q,       KC_W,       KC_E,       KC_R,       KC_T,
        KC_CAPS,    KC_A,       KC_S,       KC_D,       KC_F,       KC_G,
        KC_LSFT,    KC_Z,       KC_X,       KC_C,       KC_V,       KC_B,
        KC_LCTL,    MO(1),      KC_LALT,    KC_SPC,     KC_VOLU,    KC_VOLD

    ),

    [1] = LAYOUT(
        //1      2        3        4        5        6        7        8        9        10       11       12       13       14       15       16       17       18       19       20       21
        _______,    _______,    _______,    _______,    _______,    _______,
        _______,    _______,    _______,    _______,    _______,    _______,    _______,    
        _______,    _______,    _______,    _______,    _______,    _______,    
        _______,    _______,    _______,    _______,    _______,    _______,    
        _______,    _______,    _______,    _______,    _______,    _______,    
        _______,    _______,    MO(2),      _______,    KC_MNXT,    KC_MPRV),
    [2] = LAYOUT(
        //1      2        3        4        5        6        7        8        9        10       11       12       13       14       15       16       17       18       19       20       21
        EE_CLR,     _______,    _______,    _______,    _______,    QK_BOOT,
        _______,    _______,    _______,    _______,    _______,    _______,    _______,    
        NK_TOGG,    RGB_VAI,    RGB_HUI,    RGB_SAI,    _______,    _______,    
        _______,    _______,    RGB_HUI,    RGB_SAI,    RGB_VAI,    RGB_SPI,
        RGB_TOG,    RGB_MOD,    RGB_HUD,    RGB_SAD,    RGB_VAD,    RGB_SPD,
        _______,    _______,    _______,    _______,    _______,    _______)
    
};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // caps lock cyan

    // num lock cyan
    if (host_keyboard_led_state().caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(18, 255, 255, 255);
    } 


    for (uint8_t i = led_min; i < led_max; i++) {
        switch(get_highest_layer(layer_state|default_layer_state)) {
            case 7:
                rgb_matrix_set_color(i, RGB_WHITE);
                break;
            case 6:
                rgb_matrix_set_color(i, RGB_PURPLE);
                break;
            case 5:
                rgb_matrix_set_color(i, RGB_YELLOW);
                break;
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
