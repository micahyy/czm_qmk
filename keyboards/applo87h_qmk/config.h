#pragma once

// ── USB Device ──
#define VENDOR_ID       0x534C  // "SL"
#define PRODUCT_ID      0x0871
#define DEVICE_VER      0x0100
#define MANUFACTURER    SLK
#define PRODUCT         Applo87H

// ── Matrix ──
#define MATRIX_ROWS     6
#define MATRIX_COLS     17
#define DIODE_DIRECTION COL2ROW

// 8K requires the matrix scan to be meaningfully faster than 125us USB microframes.
// 6x17 = 102 diodes; keep GPIO settling delay short while remaining stable.
#define MATRIX_IO_DELAY 1

// ── Debounce ──
// sym_eager_pk reports press/release immediately; 1ms validation window.
#define DEBOUNCE 1

// ── NKRO ──
#define FORCE_NKRO

// ── USB 8000 Hz polling ──
// AT32F405 has an independent OTGHS controller with on-chip USB 2.0 HS PHY.
// The schematic routes USB connector to OTGHS1_D+/OTGHS1_D-.
// In HS mode bInterval=1 means 1 microframe = 125us = 8000 Hz.
#define USB_POLLING_INTERVAL_MS 1
#define USB_MAX_POWER_CONSUMPTION 500

// ── WS2812 underglow ──
// PC6 = TIM3_CH1, PWM DMA.
#define RGBLIGHT_LED_COUNT   32
#define RGBLIGHT_LIMIT_VAL   180

// ── LED indicators ──
// CAPS_LED = PA2, SL_LED = PD2, schematic transistor circuit is active high.
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
