# List of all the board related files.
BOARDSRC = $(BOARD_PATH)/boards/GENERIC_AT32_F405XX/board.c

# Required include directories (board files + configs for mcuconf.h)
BOARDINC = $(BOARD_PATH)/boards/GENERIC_AT32_F405XX $(BOARD_PATH)/boards/GENERIC_AT32_F405XX/configs

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
