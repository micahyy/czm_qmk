#pragma once

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

// ── USB 8000 Hz polling ──
// AT32F405 has an independent OTGHS controller with on-chip USB 2.0 HS PHY.
// The schematic routes USB connector to OTGHS1_D+/OTGHS1_D-.
// In HS mode bInterval=1 means 1 microframe = 125us = 8000 Hz.
#define USB_POLLING_INTERVAL_MS 1
#define USB_MAX_POWER_CONSUMPTION 500

// ── LED indicators ──
// CAPS_LED = PA2, SL_LED = PD2, schematic transistor circuit is active high.
#define LED_CAPS_LOCK_PIN    A2
#define LED_SCROLL_LOCK_PIN  D2
#define LED_PIN_ON_STATE     1

// ── Tapping ──
#define TAPPING_TERM 200
#define PERMISSIVE_HOLD

// ── Bootmagic (hold Esc while plugging in) ──
#define BOOTMAGIC_LITE_ROW    0
#define BOOTMAGIC_LITE_COLUMN 0

// ── USB: use OTGHS (USBD2) for 480Mbps High Speed ──
#define USB_DRIVER USBD2

// ── VIA / Dynamic Keymap EEPROM ──
// AT32F405 emulated EEPROM via wear-leveling in Flash.
// 4 layers × 87 keys × 2 bytes = 696 bytes for keymaps.
// Reserve 2048 bytes total, leaving room for macros and VIA config.
#define WEAR_LEVELING_LOGICAL_SIZE 2048
#define WEAR_LEVELING_BACKING_SIZE 4096
#define WEAR_LEVELING_EFL_FLASH_SIZE (256 * 1024)
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR 2047
#define DYNAMIC_KEYMAP_LAYER_COUNT 4
