/* Copyright 2026 CZMAO
 *
 * CherryUF2 bootloader entry for AT32F403A.
 *
 * CherryUF2 (zhaqian12/Cherryuf2) uses a RAM magic + reset protocol:
 *   - On reset the bootloader checks the double-tap word at the very
 *     top of SRAM (the initial-MSP location). If it holds the enter-DFU
 *     magic, the bootloader stays in UF2/MSC mode; otherwise it jumps to
 *     the application at CONFIG_BOOTUF2_APP_START_ADDR = 0x08004000.
 *   - The application simply writes the magic word and performs a system
 *     reset to enter the UF2 bootloader (same path as VIA "reset to
 *     bootloader" / the QK_BOOT keycode).
 *
 * For AT32F403ARCT7: SRAM = 96K, and CherryUF2 links its stack at
 * (RAM_BASE + 96K - 4) = 0x20017FFC, which is the double-tap register.
 */

#include "bootloader.h"
#include <ch.h>

#define CHERRYUF2_DBL_TAP_REG   (*(volatile uint32_t *)0x20017FFCUL)
#define CHERRYUF2_DBL_TAP_MAGIC 0xf01669efUL

void bootloader_jump(void) {
    CHERRYUF2_DBL_TAP_REG = CHERRYUF2_DBL_TAP_MAGIC;
    NVIC_SystemReset();
}

void mcu_reset(void) {
    NVIC_SystemReset();
}
