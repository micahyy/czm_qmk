/* Copyright 2026 CZMAO
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include QMK_KEYBOARD_H

/*
 * Host compatibility switches for the Gamer ST (08gamerst_uf2).
 *
 * The board is also sold to be plugged into phone game-throne adapters
 * (猎影 / 暗影王座) and directly into phone OTG. Those hosts only cope with a
 * plain 6KRO boot-protocol keyboard, so the firmware leaves NKRO off out of
 * the box. Mouse keys are compiled out entirely (see keyboard.json) because a
 * mouse HID collection in the descriptor makes throne firmware bind the whole
 * device to the mouse path, and the keyboard then "moves the cursor" instead
 * of typing.
 *
 * Media / system keys stay enabled by default: the rotary encoder is a selling
 * point of this board and needs them. They live in their own HID collection,
 * so they cannot confuse the keyboard path. The VIA switch only exists as an
 * escape hatch if a host misbehaves.
 *
 * Every setting is stored in EEPROM and survives power cycles.
 */

#define CZM_FLAG_MAGIC 0xA5000000u
#define CZM_FLAG_MASK  0xFF000000u
#define CZM_FLAG_EXTRA 0x02u

/* Fresh EEPROM: media keys on, NKRO off. */
#define CZM_FLAG_DEFAULT (CZM_FLAG_MAGIC | CZM_FLAG_EXTRA)

static uint32_t czm_flags;

static void czm_flags_load(void) {
    czm_flags = eeconfig_read_user();

    if ((czm_flags & CZM_FLAG_MASK) != CZM_FLAG_MAGIC) {
        czm_flags = CZM_FLAG_DEFAULT;
        eeconfig_update_user(czm_flags);

        /* Do not inherit NKRO from an older firmware. */
        keymap_config.nkro = false;
        eeconfig_update_keymap(&keymap_config);
    }
}

#ifdef VIA_ENABLE

static void czm_flags_set(uint32_t flag, bool on) {
    if (on) {
        czm_flags |= flag;
    } else {
        czm_flags &= ~flag;
    }
    eeconfig_update_user(czm_flags);
}

/* VIA "Keyboard" tab controls.
 * Custom channel is id_custom_channel (0); value ids must match the
 * "content" arrays in the VIA definition json: [name, channel, valueId].
 * id 4 is reserved for the mouse keys switch GamerLite uses; this board
 * compiles mouse support out, so the slot stays unused. */
enum via_czm_value {
    id_czm_reset_kb      = 1, /* Reset keymap to defaults (action) */
    id_czm_bootloader_kb = 2, /* Enter bootloader (action) */
    id_czm_nkro_kb       = 3, /* NKRO on/off (0 = 6KRO, 1 = NKRO) */
    id_czm_extrakey_kb   = 5, /* Media / system keys on/off, default on */
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
                case id_czm_reset_kb:
                case id_czm_bootloader_kb:
                    /* Action-only buttons are still read with a GET on menu
                     * load; answer 0 so VIA does not reject the menu. */
                    *value_data = 0;
                    break;
                case id_czm_nkro_kb:
                    *value_data = keymap_config.nkro ? 1 : 0;
                    break;
                case id_czm_extrakey_kb:
                    *value_data = (czm_flags & CZM_FLAG_EXTRA) ? 1 : 0;
                    break;
                default:
                    *command_id = id_unhandled;
                    break;
            }
            break;

        case id_custom_set_value:
            switch (*value_id) {
                case id_czm_reset_kb:
                    eeconfig_init_via();
                    clear_keyboard();
                    soft_reset_keyboard();
                    break;
                case id_czm_bootloader_kb:
                    clear_keyboard();
                    reset_keyboard();
                    break;
                case id_czm_nkro_kb:
                    clear_keyboard();
                    keymap_config.nkro = (*value_data != 0);
                    eeconfig_update_keymap(&keymap_config);
                    clear_keyboard();
                    break;
                case id_czm_extrakey_kb:
                    czm_flags_set(CZM_FLAG_EXTRA, (*value_data != 0));
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

#endif /* VIA_ENABLE */

/* Swallow media / system keycodes when the switch is off, so no consumer or
 * system-control report is ever sent to a phone or throne host. */
bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!(czm_flags & CZM_FLAG_EXTRA) && (IS_SYSTEM_KEYCODE(keycode) || IS_CONSUMER_KEYCODE(keycode))) {
        return false;
    }

    return process_record_user(keycode, record);
}

void keyboard_post_init_kb(void) {
    czm_flags_load();
    keyboard_post_init_user();
}
