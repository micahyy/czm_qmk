# VIA features (same as the "via" keymap)
VIA_ENABLE = yes

# CherryUF2 bootloader target for STM32F103CBT6 (Gamer ST, UF2 build).
# NOTE: keyboard.json keeps bootloader=stm32duino ONLY so QMK resolves the
# existing STM32_F103_STM32DUINO board directory (there is no
# GENERIC_STM32_F103 board here). The real on-chip bootloader is CherryUF2;
# its jump routine (RAM magic 0xf01669ef @ 0x20004FFC + reset) lives in
# cherryuf2_bootloader.c and overrides the weak stm32duino bootloader_jump.
# - application is relinked at 0x08000000 + 16K via MCU_LDSCRIPT
# - output a .uf2 drag-and-drop image
MCU_LDSCRIPT = STM32F103xB_uf2boot
FIRMWARE_FORMAT = uf2

# CherryUF2 UF2 family ID. This MUST equal CONFIG_BOOTUF2_FAMILYID baked
# into the on-chip CherryUF2 bootloader. The stock ST stm32f1xx port builds
# with UF2_FAMILY_ID = 0x5ee21072 (see Cherryuf2/ports/ST/stm32f1xx/port.mk),
# which is what the prebuilt cherryuf2_stm32f103cbt6.bin uses, so we match it
# here to allow drag-and-drop without reflashing the bootloader.
UF2_FAMILY = 0x5ee21072

SRC += cherryuf2_bootloader.c

LTO_ENABLE = yes
