#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,
        KC_LCTL, MO(1),   KC_LALT, KC_SPC,  KC_NO,   KC_NO
    ),

    [1] = LAYOUT(
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, MO(2),   _______, _______, _______
    ),

    [2] = LAYOUT(
        EE_CLR,  _______, _______, _______, _______, QK_BOOT,
        _______, _______, _______, _______, _______, _______, _______,
        NK_TOGG, RM_VALU, RM_HUEU, RM_SATU, _______, _______,
        _______, _______, RM_HUED, RM_SATD, RM_VALD, RM_SPDU,
        RM_TOGG, RM_NEXT, RM_HUED, RM_SATD, RM_VALD, RM_SPDD,
        _______, _______, _______, _______, _______, _______
    ),

    [3] = LAYOUT(
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
    ),

    [4] = LAYOUT(
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
    ),

    [5] = LAYOUT(
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
    ),

    [6] = LAYOUT(
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
    ),

    [7] = LAYOUT(
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [1] = { ENCODER_CCW_CW(KC_MPRV, KC_MNXT) },
    [2] = { ENCODER_CCW_CW(RM_VALD, RM_VALU) },
    [3] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [4] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [5] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [6] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [7] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
};
#endif

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (host_keyboard_led_state().caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(20, 255, 255, 255);
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        switch (get_highest_layer(layer_state | default_layer_state)) {
            case 7:  rgb_matrix_set_color(i, RGB_WHITE);  break;
            case 6:  rgb_matrix_set_color(i, RGB_PURPLE); break;
            case 5:  rgb_matrix_set_color(i, RGB_YELLOW); break;
            case 4:  rgb_matrix_set_color(i, RGB_PINK);   break;
            case 3:  rgb_matrix_set_color(i, RGB_BLUE);   break;
            case 2:  rgb_matrix_set_color(i, RGB_GREEN);  break;
            case 1:  rgb_matrix_set_color(i, RGB_RED);    break;
            default: break;
        }
    }
    return false;
}
