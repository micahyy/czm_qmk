#include QMK_KEYBOARD_H
#include "gpio.h"
#include "wait.h"  // <-- 新增这一行！

void keyboard_pre_init_user(void) {
    setPinOutput(B13);
    writePinLow(B13);
    wait_us(300);  // 现在可以正常使用
}
void matrix_init_user(void) {
    rgb_matrix_set_color_all(0, 0, 0);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_numberpad_5x4(
        LT(1,KC_NUM), KC_PSLS, KC_PAST, KC_PMNS,
        KC_P7,        KC_P8,   KC_P9,   
        KC_P4,        KC_P5,   KC_P6,   KC_PPLS,
        KC_P1,        KC_P2,   KC_P3,   
        KC_P0,                 KC_PDOT, KC_PENT
    ),
    [1] = LAYOUT_numberpad_5x4(
        KC_TRNS, KC_TRNS, MO(2),   RGB_VAI,
        RGB_TOG, RGB_SAI, KC_TRNS, 
        RGB_HUD, KC_TRNS, RGB_HUI, RGB_VAD,  
        KC_TRNS, RGB_SAD, KC_TRNS, 
        RGB_MOD,          KC_TRNS, KC_TRNS
    ),
    [2] = LAYOUT_numberpad_5x4(
        KC_TRNS, KC_TRNS, KC_TRNS, NK_TOGG,
        KC_TRNS, KC_TRNS, KC_TRNS, 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, 
        KC_TRNS,          KC_TRNS, KC_TRNS
    ),
    [7] = LAYOUT_numberpad_5x4(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, 
        KC_TRNS,          KC_TRNS, EE_CLR
    )
};



#include "rgb_matrix.h"
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // caps lock cyan

    // num lock cyan
    if (host_keyboard_led_state().num_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(0, 255, 255, 255);
    } 



    // layer state
     for (uint8_t i = led_min; i < led_max; i++) {
        switch(get_highest_layer(layer_state|default_layer_state)) {
            case 7:
                rgb_matrix_set_color(i, RGB_PURPLE);
                break;
            case 6:
                rgb_matrix_set_color(i, RGB_WHITE);
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

