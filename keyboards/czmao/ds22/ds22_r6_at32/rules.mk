# DS22 R6 AT32 — AT32F403ACCT7 (LQFP48, 256KB Flash, 96KB SRAM)
# Bootloader: CherryUF2 at32f403a port, 16KB @ 0x08000000, app @ 0x08004000
#
# QMK's tinyuf2.c writes the CherryUF2 double-tap magic (0xf01669ef) to
# _board_dfu_dbl_tap (defined in ld/AT32F403AxC.ld at top-of-RAM) before
# resetting, so QK_BOOT / Bootmagic can enter the UF2 flashing drive.
BOOTLOADER = tinyuf2

USE_CHIBIOS_CONTRIB = yes
USE_FPU      = yes
LTO_ENABLE   = yes

# QMK's uf2conv.py does not know the AT32 family id; CI converts the .bin
# with scripts/bin2uf2.py (base 0x08004000, family 0x98a285b9) instead.
FIRMWARE_FORMAT = bin

# Clock comes from boards/GENERIC_AT32_F403A/configs/mcuconf.h:
# HEXT 8MHz x24 = 192MHz SYSCLK, USB = PLL/4 = 48MHz, FS device @ PA11/PA12.

# EEPROM emulation via Flash wear-leveling (last 4KB page @ 0x0803F000)
EEPROM_DRIVER = wear_leveling
WEAR_LEVELING_DRIVER = embedded_flash

# WS2812: B13 has no regular TMRx_CHx mux on AT32F403A (only TMR1_CH1N
# complementary output, unsupported by ws2812_pwm) — use the bitbang driver.
# The driver natively supports AT32F403Axx (NOP_FUDGE 0.4 default); verify
# colors on hardware and tune if needed (see Rush60 192MHz notes).
WS2812_DRIVER = bitbang
