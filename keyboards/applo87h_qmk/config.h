#pragma once

// ── Matrix ──
#define MATRIX_ROWS     6
#define MATRIX_COLS     17
#define DIODE_DIRECTION COL2ROW
#define MATRIX_IO_DELAY 1

// ── Debounce ──
#define DEBOUNCE 1

// ── USB 8000 Hz polling ──
#define USB_POLLING_INTERVAL_MS 1
#define USB_MAX_POWER_CONSUMPTION 500

// ── WS2812 ──
#define RGBLIGHT_LED_COUNT   32
#define RGBLIGHT_LIMIT_VAL   180

// ── LED indicators ──
#define LED_CAPS_LOCK_PIN    A2
#define LED_SCROLL_LOCK_PIN  D2
#define LED_PIN_ON_STATE     1

// ── RGB defaults ──
#define RGBLIGHT_DEFAULT_MODE     RGBLIGHT_MODE_STATIC_LIGHT
#define RGBLIGHT_DEFAULT_HUE      128
#define RGBLIGHT_DEFAULT_SAT      255
#define RGBLIGHT_DEFAULT_VAL      128
#define RGBLIGHT_DEFAULT_SPD      0

// ── Tapping ──
#define TAPPING_TERM 200
#define PERMISSIVE_HOLD

// ── Bootmagic ──
#define BOOTMAGIC_LITE_ROW    0
#define BOOTMAGIC_LITE_COLUMN 0
