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

#include QMK_KEYBOARD_H

enum custom_keycodes {
    EC_EEPRST = QK_USER_0,
    EC_RGB_TOG = QK_USER_1,
    EC_RGB_NEXT = QK_USER_2,
    EC_RGB_PREV = QK_USER_3,
    EC_RGB_HUEU = QK_USER_4,
    EC_RGB_HUED = QK_USER_5,
    EC_RGB_SATU = QK_USER_6,
    EC_RGB_SATD = QK_USER_7,
    EC_RGB_VALU = QK_USER_8,
    EC_RGB_VALD = QK_USER_9,
    EC_RGB_SPDU = QK_USER_10,
    EC_RGB_SPDD = QK_USER_11,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        KC_ESC , KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 ,  KC_PSCR, KC_SCRL, KC_PAUS,
        KC_GRV , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_MINS, KC_EQL ,  KC_BSPC, KC_INS , KC_HOME,
        KC_PGUP,
        KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_LBRC, KC_RBRC,  KC_BSLS,
        KC_ENT , KC_DEL , KC_END ,
        KC_PGDN,
        KC_CAPS, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT, KC_NUHS,
        KC_LSFT, KC_NUBS, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT,  MO(2)  , KC_UP  ,
        KC_LCTL, KC_LGUI, KC_LALT, KC_SPC , KC_SPC , KC_SPC , KC_RALT, KC_RGUI, MO(1)  , KC_RCTL,
        KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [1] = LAYOUT(
        KC_GRV , KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU,  _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  KC_DEL , _______, _______,
        _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,
        _______, _______, _______,
        _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______, KC_PGUP,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_HOME, KC_PGDN, KC_END
    ),

    [2] = LAYOUT(
        EC_EEPRST,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,_______,_______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,_______,_______,
        _______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,
        _______,QK_BOOT,_______,
        _______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
        EC_RGB_TOG,_______,EC_RGB_VALD,EC_RGB_VALU,EC_RGB_HUED,EC_RGB_HUEU,EC_RGB_SATD,EC_RGB_SATU,EC_RGB_SPDD,EC_RGB_SPDU,EC_RGB_NEXT,EC_RGB_PREV,_______,  _______,_______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
        _______,_______,_______
    ),

    [3] = LAYOUT(
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,_______,_______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,_______,_______,
        _______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,
        _______,_______,_______,
        _______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,_______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
        _______,_______,_______
    )

};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) { return true; }

    switch (keycode) {
        case EC_EEPRST:
            eeconfig_init();
            soft_reset_keyboard();
            return false;
        case EC_RGB_TOG:
            rgb_matrix_toggle();
            return false;
        case EC_RGB_NEXT:
            rgb_matrix_step();
            return false;
        case EC_RGB_PREV:
            rgb_matrix_step_reverse();
            return false;
        case EC_RGB_HUEU:
            rgb_matrix_increase_hue();
            return false;
        case EC_RGB_HUED:
            rgb_matrix_decrease_hue();
            return false;
        case EC_RGB_SATU:
            rgb_matrix_increase_sat();
            return false;
        case EC_RGB_SATD:
            rgb_matrix_decrease_sat();
            return false;
        case EC_RGB_VALU:
            rgb_matrix_increase_val();
            return false;
        case EC_RGB_VALD:
            rgb_matrix_decrease_val();
            return false;
        case EC_RGB_SPDU:
            rgb_matrix_increase_speed();
            return false;
        case EC_RGB_SPDD:
            rgb_matrix_decrease_speed();
            return false;
    }
    return true;
}
