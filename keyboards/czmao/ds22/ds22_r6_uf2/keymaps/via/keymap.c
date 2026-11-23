// Copyright 2024 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "wait.h"
#include "eeconfig.h"
#include "raw_hid.h"

/* VIA "功能" tab custom commands (value ids must match the JSON menus).
 * data = [ command_id, channel_id, value_id, value_data ] */
enum via_czm_value {
    id_czm_reset_kb      = 1, /* 恢复出厂设置 (action) */
    id_czm_bootloader_kb = 2, /* 进入 Bootloader (action) */
    id_czm_nkro_kb       = 3, /* 全键无冲 NKRO on/off (0=6KRO, 1=NKRO) */
};

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
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
                case id_czm_reset_kb:
                case id_czm_bootloader_kb:
                    /* Action-only buttons are read on menu load; answer 0. */
                    *value_data = 0;
                    break;
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
                    /* Factory reset = system QK_CLEAR_EEPROM behaviour: disable
                     * eeconfig and soft-reset. Reboots into the firmware and
                     * reloads defaults. */
                    eeconfig_disable();
                    clear_keyboard();
                    soft_reset_keyboard();
                    break;
                case id_czm_bootloader_kb:
                    /* Enter bootloader = system QK_BOOT behaviour.
                     * bootloader.c's bootloader_jump() override writes the
                     * CherryUF2 double-tap magic, so this lands in the UF2
                     * drive. */
                    reset_keyboard();
                    break;
                case id_czm_nkro_kb:
                    clear_keyboard();
                    keymap_config.nkro = (*value_data != 0);
                    eeconfig_update_keymap(keymap_config.raw);
                    clear_keyboard();
                    break;
                default:
                    *command_id = id_unhandled;
                    break;
            }
            break;
        case id_custom_save:
            break;
        default:
            *command_id = id_unhandled;
            break;
    }
}

/* VIA web (v3) probes the keyboard name (id_get_keyboard_value +
 * id_qmk_keyboard_name = 0x06) right after the protocol-version handshake.
 * QMK v21's via.c predates that command and answers 0xFF, which hangs the
 * VIA web loading screen forever. Answer it with the product string. Also
 * answer the get direction of device indication (0x05) with 0, which newer
 * VIA apps read while connecting; the set direction is already handled by
 * via.c. Anything else falls through to the stock handler (which also
 * dispatches via_custom_value_command_kb above). */
#define VIA_ID_DEVICE_INDICATION 0x05
#define VIA_ID_QMK_KEYBOARD_NAME 0x06

bool via_command_kb(uint8_t *data, uint8_t length) {
    if (data[0] == id_get_keyboard_value) {
        if (data[1] == VIA_ID_QMK_KEYBOARD_NAME) {
            memset(data, 0, length);
            data[0] = id_get_keyboard_value;
            data[1] = VIA_ID_QMK_KEYBOARD_NAME;
            strlcpy((char *)&data[2], PRODUCT, length - 2);
            raw_hid_send(data, length);
            return true;
        }
        if (data[1] == VIA_ID_DEVICE_INDICATION) {
            memset(data, 0, length);
            data[0] = id_get_keyboard_value;
            data[1] = VIA_ID_DEVICE_INDICATION;
            raw_hid_send(data, length);
            return true;
        }
    }
    return false;
}

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

// 指示灯：FN 按下 → RGB19（FN 键，LED18）白；NumLock → RGB2（NUM 键，LED1）白。
// FN 状态由按键事件维护，keymap 不引用 layer 子系统（实测引用即触发 WS2812 位流错乱）
static bool czm_fn_active = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MO(1):
        case LT(1, KC_NUM):
            czm_fn_active = record->event.pressed;
            break;
    }
    return true;
}

bool rgb_matrix_indicators_user(void) {
    if (host_keyboard_led_state().num_lock) {
        rgb_matrix_set_color(1, 255, 255, 255);
    }
    if (czm_fn_active) {
        rgb_matrix_set_color(18, 255, 255, 255);
    }
    return true;
}
