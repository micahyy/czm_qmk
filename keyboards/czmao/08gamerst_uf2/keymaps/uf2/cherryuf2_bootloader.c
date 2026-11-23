/* Copyright 2026 CZMAO
 *
 * CherryUF2 bootloader entry for STM32F103CBT6 (Gamer ST, UF2 build).
 *
 * CherryUF2 (zhaqian12/Cherryuf2) uses a RAM magic + reset protocol:
 *   - On reset the bootloader checks the double-tap word at the very
 *     top of SRAM. For STM32F103xB CherryUF2 links its stack at
 *     (RAM_BASE + 20K - 4) = 0x20004FFC, which is the double-tap register.
 *     If it holds the enter-DFU magic the bootloader stays in UF2/MSC mode,
 *     otherwise it jumps to the application at 0x08004000.
 *   - The application simply writes the magic word and performs a system
 *     reset to enter the UF2 bootloader (QK_BOOT keycode path).
 *
 * The Cathouse one-key upgrade flow sends VIA command id 0x0b
 * (id_bootloader_jump) over raw HID. It lands here through the weak
 * via_command_kb() hook, so the frontend can jump into the bootloader without
 * a physical double-tap.
 *
 * Interrupts are masked before the store as insurance: the word lives at the
 * top of SRAM, and this app links with the platform STM32F103xB_uf2boot.ld
 * whose ram0 is the full 20K, so nothing reserves it. The ChibiOS main stack
 * currently starts low (the vector table's initial SP is 0x20000400), so no
 * ISR pushes onto 0x20004FFC today -- but if a future ld/board change moves
 * the stack to the top, a tick landing between the store and the reset would
 * silently eat the magic and the boot would skip DFU. Masking costs nothing.
 */

#include "bootloader.h"
#include "quantum.h"
#include "via.h"

#define CHERRYUF2_DBL_TAP_REG   (*(volatile uint32_t *)0x20004FFCUL)
#define CHERRYUF2_DBL_TAP_MAGIC 0xf01669efUL

void bootloader_jump(void) {
    __disable_irq();
    CHERRYUF2_DBL_TAP_REG = CHERRYUF2_DBL_TAP_MAGIC;
    __DSB();
    NVIC_SystemReset();
    while (1) {}
}

void mcu_reset(void) {
    NVIC_SystemReset();
}

/* VIA raw-HID command dispatcher override.
 * data[0] is the command id. We intercept 0x0b (bootloader jump); every
 * other command falls through to the stock VIA handler. */
bool via_command_kb(uint8_t *data, uint8_t length) {
    if (length > 0 && data[0] == 0x0B) {
        bootloader_jump();
        return true;
    }
    return false;
}
