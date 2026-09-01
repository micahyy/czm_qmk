# VIA features (same as the "via" keymap)
VIA_ENABLE = yes

# CherryUF2 bootloader target for AT32F403A
# - custom bootloader: jump routine provided in cherryuf2_bootloader.c
# - application is linked at 0x08004000 (first 16KB reserved for the
#   CherryUF2 bootloader, which lives at 0x08000000)
# - output a .uf2 drag-and-drop image
BOOTLOADER = custom
MCU_LDSCRIPT = AT32F403AxC_uf2
FIRMWARE_FORMAT = uf2

# CherryUF2 UF2 family ID for "AT32F403_7" (from CherryUF2 / uf2families.json)
UF2_FAMILY = 0x98a285b9

SRC += cherryuf2_bootloader.c

LTO_ENABLE = yes
