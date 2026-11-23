/*
 * AT32F403AxC memory setup — CZMAO DS22 R6 with CherryUF2 bootloader.
 *
 * Flash layout (AT32F403ACCT7, 256KB):
 *   0x08000000 - 0x08003FFF  16KB  CherryUF2 bootloader
 *   0x08004000 - 0x0803EFFF  236KB Application (QMK)
 *   0x0803F000 - 0x0803FFFF  4KB   Wear-leveling / EEPROM emulation
 */
MEMORY
{
    flash0 (rx) : org = 0x08004000, len = 240k
    flash1 (rx) : org = 0x00000000, len = 0
    flash2 (rx) : org = 0x00000000, len = 0
    flash3 (rx) : org = 0x00000000, len = 0
    flash4 (rx) : org = 0x00000000, len = 0
    flash5 (rx) : org = 0x00000000, len = 0
    flash6 (rx) : org = 0x00000000, len = 0
    flash7 (rx) : org = 0x00000000, len = 0
    ram0   (wx) : org = 0x20000000, len = 96k - 4
    ram1   (wx) : org = 0x00000000, len = 0
    ram2   (wx) : org = 0x00000000, len = 0
    ram3   (wx) : org = 0x00000000, len = 0
    ram4   (wx) : org = 0x00000000, len = 0
    ram5   (wx) : org = 0x00000000, len = 0
    ram6   (wx) : org = 0x00000000, len = 0
    ram7   (wx) : org = 0x00000000, len = 0
}

REGION_ALIAS("VECTORS_FLASH", flash0);
REGION_ALIAS("VECTORS_FLASH_LMA", flash0);
REGION_ALIAS("XTORS_FLASH", flash0);
REGION_ALIAS("XTORS_FLASH_LMA", flash0);
REGION_ALIAS("TEXT_FLASH", flash0);
REGION_ALIAS("TEXT_FLASH_LMA", flash0);
REGION_ALIAS("RODATA_FLASH", flash0);
REGION_ALIAS("RODATA_FLASH_LMA", flash0);
REGION_ALIAS("VARIOUS_FLASH", flash0);
REGION_ALIAS("VARIOUS_FLASH_LMA", flash0);
REGION_ALIAS("RAM_INIT_FLASH_LMA", flash0);

REGION_ALIAS("MAIN_STACK_RAM", ram0);
REGION_ALIAS("PROCESS_STACK_RAM", ram0);
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash0);
REGION_ALIAS("BSS_RAM", ram0);
REGION_ALIAS("HEAP_RAM", ram0);

INCLUDE rules.ld

/* CherryUF2 double-tap magic register.
 * QMK's tinyuf2.c writes DBL_TAP_MAGIC (0xf01669ef) to this exact address
 * before resetting; the bootloader's check_dfu_mode() reads it on boot.
 * Address must match the bootloader's ld: RAM origin 0x20000000 + (96K - 4). */
_board_dfu_dbl_tap = ORIGIN(ram0) + 96K - 4;
