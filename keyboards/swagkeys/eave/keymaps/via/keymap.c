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
#include "quantum/rgblight/rgblight.h"

// 标志位，标记是否已经完成初始化
static bool rgb_init_complete = false;

void keyboard_pre_init_user(void) {
    // 在 GPIO 初始化后立即复位 WS2812
    setPinOutput(B13);
    
    // 发送长复位信号，清空 WS2812 的内部状态
    writePinLow(B13);
    wait_ms(5);              // 5ms 低电平，确保所有 LED 复位
    writePinHigh(B13);
    wait_ms(50);
}

void keyboard_post_init_user(void) {
    // 延迟等待系统稳定
    wait_ms(100);
    
    // 初始化 RGB 系统（会从 EEPROM 读取之前的设置）
    rgblight_init();
    
    // 关键：在读取 EEPROM 设置后，强制刷新一次所有 LED
    // 这样会覆盖上电时可能出现的错误状态
    rgblight_set();
    
    // 标记初始化完成
    rgb_init_complete = true;
}

// 可选：在任务循环中再次确保（有些情况下需要）
void housekeeping_task_user(void) {
    static bool rgb_init_complete = false;
    
    // 在启动后 500ms 再次检查，确保 WS2812 状态正确
    if (!rgb_init_complete && timer_read32() > 500) {
        rgblight_set();  // 再次刷新
        rgb_init_complete = true;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_65_ansi(
        KC_ESC,  KC_1,    KC_2,    KC_3,   KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,   KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_BSLS, KC_BSPC,  KC_DEL,  
        KC_TAB,  KC_Q,    KC_W,    KC_E,   KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_HOME,  
        KC_CAPS, KC_A,    KC_S,    KC_D,   KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,   KC_SCLN, KC_QUOT, KC_ENT,                    KC_PGUP,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,   KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_PGDN,
        KC_LCTL, KC_LGUI, KC_LALT,                           KC_SPC,                            KC_RALT, MO(1),            KC_LEFT, KC_DOWN, KC_RGHT),

    [1] = LAYOUT_65_ansi(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,  KC_F4,    KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,  KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  
        KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_MPLY, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS,                   KC_TRNS,
        KC_TRNS,          RGB_TOG, RGB_MOD,KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS,          KC_TRNS, KC_VOLU, KC_TRNS,
        KC_TRNS, GU_TOGG, KC_TRNS,                           KC_TRNS,                            MO(2),   KC_TRNS,          KC_MPRV, KC_VOLD, KC_MNXT),
    [2] = LAYOUT_65_ansi(
        EE_CLR,  KC_TRNS, KC_TRNS,  KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,QK_BOOT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  
        KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  
        KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS,                   KC_TRNS,
        KC_TRNS,          KC_TRNS,  KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS,
        AG_TOGG, KC_TRNS, KC_TRNS,                           KC_TRNS,                            KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS)

};

    
