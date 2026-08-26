/* Copyright 2026 micahyy */
#include QMK_KEYBOARD_H

enum custom_keycodes {
    EC_BOOT = SAFE_RANGE,
    EC_EEPRST,
};

static bool fn_pressed = false;
static bool ralt_pressed = false;
static bool lctrl_pressed = false;
static bool esc_pressed = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        KC_ESC     , KC_F1      , KC_F2      , KC_F3      , KC_F4      , KC_F5      , KC_F6      , KC_F7      , KC_F8      , KC_F9      , KC_F10     , KC_F11     , KC_F12     , KC_PSCR    , KC_SCRL    , KC_PAUS    ,
        KC_GRV     , KC_1       , KC_2       , KC_3       , KC_4       , KC_5       , KC_6       , KC_7       , KC_8       , KC_9       , KC_0       , KC_MINS    , KC_EQL     , KC_BSPC    , KC_INS     , KC_HOME    ,
        KC_PGUP    ,
        KC_TAB     , KC_Q       , KC_W       , KC_E       , KC_R       , KC_T       , KC_Y       , KC_U       , KC_I       , KC_O       , KC_P       , KC_LBRC    , KC_RBRC    , KC_BSLS    ,
        KC_ENT     , KC_DEL     , KC_END     ,
        KC_PGDN    ,
        KC_CAPS    , KC_A       , KC_S       , KC_D       , KC_F       , KC_G       , KC_H       , KC_J       , KC_K       , KC_L       , KC_SCLN    , KC_QUOT    , KC_NUHS    ,
        KC_LSFT    , KC_NUBS    , KC_Z       , KC_X       , KC_C       , KC_V       , KC_B       , KC_N       , KC_M       , KC_COMM    , KC_DOT     , KC_SLSH    , KC_RSFT    , MO(1)      , KC_UP      ,
        KC_LCTL    , KC_LGUI    , KC_LALT    , KC_NO      , KC_SPC     , KC_NO      , KC_RALT    , KC_RGUI    , MO(1)      , KC_RCTL    ,
        KC_LEFT    , KC_DOWN    , KC_RGHT    
    ),

    [1] = LAYOUT(
        KC_GRV     , KC_BRID    , KC_BRIU    , KC_MCTL    , KC_LPAD    , _______    , _______    , KC_MPRV    , KC_MPLY    , KC_MNXT    , KC_MUTE    , KC_VOLD    , KC_VOLU    , _______    , _______    , _______    ,
        _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , KC_DEL     , _______    , _______    ,
        _______    ,
        _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    ,
        _______    , _______    , _______    ,
        _______    ,
        _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    ,
        _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , KC_PGUP    ,
        _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    , _______    ,
        KC_HOME    , KC_PGDN    , KC_END     
    )

};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t r = record->event.key.row;
    uint8_t c = record->event.key.col;
    if (r == 5 && c == 12) fn_pressed = record->event.pressed;
    if (r == 5 && c == 10) ralt_pressed = record->event.pressed;
    if (r == 5 && c == 0)  lctrl_pressed = record->event.pressed;
    if (r == 0 && c == 0)  esc_pressed = record->event.pressed;
    if (fn_pressed && ralt_pressed && lctrl_pressed) {
        fn_pressed = ralt_pressed = lctrl_pressed = false;
        reset_keyboard();
        return false;
    }
    if (fn_pressed && ralt_pressed && esc_pressed) {
        fn_pressed = ralt_pressed = esc_pressed = false;
        eeconfig_init();
        soft_reset_keyboard();
        return false;
    }
    return true;
}
