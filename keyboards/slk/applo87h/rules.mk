# AT32F405RC (LQFP64, 256KB Flash, 96KB SRAM, OTG-HS)
# MCU=AT32F415 triggers QMK's mcu_selection block; the board-level
# boards/GENERIC_AT32_F405XX/rules.mk then overrides MCU_SERIES to
# AT32F402_405 (the correct HAL port with OTG2/HS registry).
MCU          = AT32F415
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
