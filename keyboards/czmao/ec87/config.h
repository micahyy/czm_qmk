#pragma once

#define USB_MAX_POWER_CONSUMPTION 500

/* Force NKRO on by default (redundant with host.default.nkro in keyboard.json,
   kept for compatibility with older QMK versions that still use FORCE_NKRO) */
#define FORCE_NKRO

/* Capacitive sensing matrix */
#define MATRIX_ROWS 6
#define MATRIX_COLS 16

/* Transient EEPROM size (VIA 4-layer dynamic keymaps) */
#define TRANSIENT_EEPROM_SIZE 8192
