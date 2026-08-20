// Copyright 2026 SLK
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Board-level configuration for GENERIC_AT32_F405XX.
 *
 * VBUS is not connected to a dedicated sensing pin on this keyboard;
 * PB13 is used as a matrix column, so VBUS sensing is ignored.
 */
#define BOARD_OTG_VBUSIG

/* Allow QMK to reorder USB endpoints for optimal FIFO allocation. */
#define USB_ENDPOINTS_ARE_REORDERABLE

/*
 * Perform bootloader jump detection in early init.
 * Holding Esc (BOOTMAGIC row 0, col 0) while plugging in enters DFU.
 */
#ifndef EARLY_INIT_PERFORM_BOOTLOADER_JUMP
#    define EARLY_INIT_PERFORM_BOOTLOADER_JUMP TRUE
#endif
