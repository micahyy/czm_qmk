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

/* VIA custom keycodes. Order must match "customKeycodes" in 1243021316.json:
 * 0 = Reset Defaults, 1 = Bootloader, 2 = NKRO Toggle. */
enum via_custom_keycodes {
    CZM_RESET = QK_USER,
    CZM_BOOT,
    CZM_NKRO,
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
        _______, _______, _______, _______, _______, _______, MO(2)  , _______, _______, _______,
        KC_HOME, KC_PGDN, KC_END
    ),

    [2] = LAYOUT(
        EE_CLR ,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,_______,_______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,_______,_______,
        _______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,  _______,
        _______,_______,_______,
        _______,
        _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
        RM_TOGG,_______,RM_VALD,RM_VALU,RM_HUED,RM_HUEU,RM_SATD,RM_SATU,RM_SPDD,RM_SPDU,RM_NEXT,RM_PREV,_______,  _______,_______,
        CZM_BOOT,_______,_______,_______,_______,_______,_______,_______,_______,_______,
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
    if (record->event.pressed) {
        switch (keycode) {
            case CZM_RESET:
                /* Reset VIA dynamic keymaps/macros back to the flash defaults,
                 * then soft-reset so VIA reconnects and reloads the keymap. */
                eeconfig_init_via();
                clear_keyboard();
                soft_reset_keyboard();
                return false;
            case CZM_BOOT:
                /* Enter UF2 bootloader for firmware flashing. */
                clear_keyboard();
                bootloader_jump();
                return false;
            case CZM_NKRO:
                /* Toggle between full NKRO and 6-key rollover. */
                clear_keyboard();
                keymap_config.nkro = !keymap_config.nkro;
                eeconfig_update_keymap(&keymap_config);
                clear_keyboard();
                return false;
        }
    }
    return true;
}

/* VIA "Keyboard" tab controls.
 * Custom channel = id_custom_channel (0). Value ids must match the
 * "content" arrays in the VIA definition json:
 *   [name, channel=0, valueId] */
enum via_czm_value {
    id_czm_reset_kb      = 1, /* Reset keymap to defaults (action) */
    id_czm_bootloader_kb = 2, /* Enter UF2 bootloader (action) */
    id_czm_nkro_kb       = 3, /* NKRO on/off toggle (0 = 6KRO, 1 = NKRO) */
};

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    /* data = [ command_id, channel_id, value_id, value_data ] */
    uint8_t *command_id = &data[0];
    uint8_t *channel_id = &data[1];
    uint8_t *value_id   = &data[2];
    uint8_t *value_data = &data[3];

    if (*channel_id != id_custom_channel) {
        *command_id = id_unhandled;
        return;
    }

    switch (*command_id) {
        case id_custom_get_value:
            switch (*value_id) {
                case id_czm_nkro_kb:
                    *value_data = keymap_config.nkro ? 1 : 0;
                    break;
                default:
                    *command_id = id_unhandled;
                    break;
            }
            break;

        case id_custom_set_value:
            switch (*value_id) {
                case id_czm_reset_kb:
                    /* Reset VIA keymaps/macros to flash defaults, then soft-reset
                     * so VIA reconnects and reloads the default keymap. */
                    eeconfig_init_via();
                    clear_keyboard();
                    soft_reset_keyboard();
                    break;
                case id_czm_bootloader_kb:
                    clear_keyboard();
                    bootloader_jump();
                    break;
                case id_czm_nkro_kb:
                    clear_keyboard();
                    keymap_config.nkro = (*value_data != 0);
                    eeconfig_update_keymap(&keymap_config);
                    clear_keyboard();
                    break;
                default:
                    *command_id = id_unhandled;
                    break;
            }
            break;

        case id_custom_save:
            /* Nothing to persist beyond keymap_config (already saved above). */
            break;

        default:
            *command_id = id_unhandled;
            break;
    }
}

/* LCTRL(5,0) + Fn/MO1(5,8) + RALT-key(5,6 which is MO2 on layer1) = bootloader. */
static bool boot_combo_active = false;

static inline bool phys_pressed(uint8_t row, uint8_t col) {
    matrix_row_t r = matrix_get_row(row);
    return (r & ((matrix_row_t)1 << col)) != 0;
}

void matrix_scan_user(void) {
    if (boot_combo_active) { return; }

    if (phys_pressed(5, 0) && phys_pressed(5, 8) && phys_pressed(5, 6)) {
        boot_combo_active = true;
        reset_keyboard();
    }
}
