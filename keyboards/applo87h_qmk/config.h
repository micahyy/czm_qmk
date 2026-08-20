#pragma once

// ── Matrix ──
#define MATRIX_ROWS     6
#define MATRIX_COLS     17
#define DIODE_DIRECTION COL2ROW

// 8K requires the matrix scan to be meaningfully faster than 125us USB microframes.
#define MATRIX_IO_DELAY 1

// ── Debounce ──
#define DEBOUNCE 1

// ── USB 8000 Hz polling ──
// AT32F405 has OTGHS with on-chip USB 2.0 HS PHY.
// In HS mode bInterval=1 means 1 microframe = 125us = 8000 Hz.
#define USB_POLLING_INTERVAL_MS 1
#define USB_MAX_POWER_CONSUMPTION 500

// ── WS2812 underglow ──
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

// ── Bootmagic (hold Esc while plugging in) ──
#define BOOTMAGIC_LITE_ROW    0
#define BOOTMAGIC_LITE_COLUMN 0
