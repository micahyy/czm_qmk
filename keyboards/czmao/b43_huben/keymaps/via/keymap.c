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
    [0] = LAYOUT_tkl_f13_ansi(
        LT(1,KC_MSEL),      LT(2, KC_MPLY),
        KC_MUTE,   
        LT(3,KC_MPRV),      LT(4,KC_MNXT),

        KC_VOLD,            KC_VOLU
),
    [1] = LAYOUT_tkl_f13_ansi(
        _______,            RGB_TOG,
        _______,   
        RGB_SPD,            RGB_SPI,

        RGB_HUD,            RGB_HUI
),
    [2] = LAYOUT_tkl_f13_ansi(
        RGB_MOD,            _______,
        _______,   
        _______,            _______,

        _______,            _______
),
    [3] = LAYOUT_tkl_f13_ansi(
        _______,            _______,
        _______,   
        _______,            _______,

        _______,            _______
),
    [7] = LAYOUT_tkl_f13_ansi(
        EE_CLR,             _______, 
        _______,   
        _______,            _______,

        _______,            _______
)




 

};

    
    
