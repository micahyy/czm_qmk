/* Copyright 2026 CZMAO
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

/* AT32F403ARCT7: 256kB flash. Explicit flash size for wear-leveling EEPROM
 * backend (the automatic FLASHSIZE_BASE detection does not cover this MCU). */
#define WEAR_LEVELING_EFL_FLASH_SIZE  (256 * 1024)
