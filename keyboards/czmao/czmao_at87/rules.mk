# CZMAO AT87 — AT32F405RCT7 (LQFP64, 256KB Flash, 96KB SRAM, OTG-HS)
# UF2 Bootloader: CherryUF2 at32f405ccu7_hs, app starts at 0x08004000
# Use AT32F415 as QMK MCU entry (same AT32F402_405 HAL series),
# then override series/ldscript/startup for AT32F405xC.
MCU          = AT32F415
MCU_FAMILY   = AT32
MCU_SERIES   = AT32F402_405
MCU_LDSCRIPT = AT32F405xC
MCU_STARTUP  = at32f402
BOARD        = GENERIC_AT32_F405XX
BOOTLOADER   = at32-dfu

USE_FPU      = yes
LTO_ENABLE   = yes

BOOTMAGIC_ENABLE = yes
MOUSEKEY_ENABLE  = yes
EXTRAKEY_ENABLE  = yes
CONSOLE_ENABLE   = no
COMMAND_ENABLE   = no
NKRO_ENABLE      = yes

DEBOUNCE_TYPE    = sym_eager_pk

SPACE_CADET_ENABLE   = no
GRAVE_ESC_ENABLE     = no
MAGIC_ENABLE         = no

# EEPROM emulation via Flash wear-leveling (AT32 has no hardware EEPROM)
EEPROM_DRIVER = wear_leveling
WEAR_LEVELING_DRIVER = embedded_flash
