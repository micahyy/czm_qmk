#pragma once

// ── Matrix ──
#define MATRIX_ROWS     6
#define MATRIX_COLS     17
#define DIODE_DIRECTION COL2ROW

// ── Debounce ──
#define DEBOUNCE 5

// ── USB 8000 Hz polling ──
#define USB_POLLING_INTERVAL_MS 1
#define USB_MAX_POWER_CONSUMPTION 500

// ── LED indicators ──
#define LED_CAPS_LOCK_PIN    A2
#define LED_SCROLL_LOCK_PIN  D2
#define LED_PIN_ON_STATE     1

// ── Tapping ──
#define TAPPING_TERM 200
#define PERMISSIVE_HOLD

// ── Bootmagic (hold Esc while plugging in) ──
#define BOOTMAGIC_LITE_ROW    0
#define BOOTMAGIC_LITE_COLUMN 0

// ── USB: use OTGHS (USBD2) ──
#define USB_DRIVER USBD2

// ── UF2 Bootloader (CherryUF2) ──
#define FLASH_BOOTLOADER_SIZE 0x4000
#define FLASH_RESERVED_SIZE   0x4000

// ── VIA / Dynamic Keymap EEPROM ──
#define WEAR_LEVELING_LOGICAL_SIZE 2048
#define WEAR_LEVELING_BACKING_SIZE 4096
#define WEAR_LEVELING_EFL_FLASH_SIZE (256 * 1024)
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR 2047
#define DYNAMIC_KEYMAP_LAYER_COUNT 4
