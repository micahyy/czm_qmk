# Generic STM32F103 board for CZMAO DS22 R6 (APM32F103CBT6).
# Placed at keyboard level because QMK upstream has no GENERIC_STM32_F103
# board directory (it was keyboard-level historically, e.g. cannonkeys
# practice60). The default BOARD from mcu_selection.mk resolves here via
# platform.mk's keyboard-level fallback: $(KEYBOARD_PATH)/boards/$(BOARD)/board.mk

# List of all the board related files.
BOARDSRC = $(BOARD_PATH)/boards/GENERIC_STM32_F103/board.c

# Required include directories (board files + configs for mcuconf.h)
BOARDINC = $(BOARD_PATH)/boards/GENERIC_STM32_F103 $(BOARD_PATH)/boards/GENERIC_STM32_F103/configs

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)

# This fork's quirk: with BOOTLOADER=custom the board stays on the default
# GENERIC_STM32_F103 chain, so the platform STM32_F103_STM32DUINO chconf
# (CH_CFG_ST_TIMEDELTA 0 = classic periodic SysTick) is NOT in the include
# path. Common chconf defaults to TIMEDELTA=2 (freerunning), which routes the
# ST driver through TIM2 on this old ChibiOS and errors out with
# `"TIM2 is not a 32bits timer"` (F103 TIM2 is 16-bit).
# Force the classic periodic tick, same as ds17r6_uf2 uses via its board.
OPT_DEFS += -DCH_CFG_ST_TIMEDELTA=0
