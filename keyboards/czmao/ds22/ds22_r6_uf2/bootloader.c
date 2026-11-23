/* DS22 R6 UF2 bootloader entry, CherryUF2 v1.1-compatible.
 *
 * CherryUF2 checks DBL_TAP_REG = _board_dfu_dbl_tap = _estack
 * (0x20000000 + 20K - 4 = 0x20004FFC) right after reset:
 *   0xf01669ef  DBL_TAP_MAGIC          -> enter UF2 mode
 *   0xf02669ef  DBL_TAP_MAGIC_QUICK_BOOT -> clear + jump to app, no 500ms window
 *   any other value                      -> set DFU magic, open 500ms double-tap
 *                                           window, clear, jump to app
 *
 * The 500ms window is the trap: writing 0 leaves boot in the "unknown" path,
 * which re-arms the DFU magic for half a second. Write QUICK_BOOT instead so
 * every soft reset takes the no-window direct-jump path.
 */
#include <stdint.h>
#include <hal.h>
#include "bootloader.h"

#define CZM_DBL_TAP_ADDR  0x20004FFCUL
#define CZM_DFU_MAGIC     0xf01669efUL
#define CZM_QUICK_MAGIC   0xf02669efUL

void bootloader_jump(void) {
    *(volatile uint32_t *)CZM_DBL_TAP_ADDR = CZM_DFU_MAGIC;
    NVIC_SystemReset();
}

void mcu_reset(void) {
    *(volatile uint32_t *)CZM_DBL_TAP_ADDR = CZM_QUICK_MAGIC;
    NVIC_SystemReset();
}

/* no two-stage reset needed */
void enter_bootloader_mode_if_requested(void) {}
