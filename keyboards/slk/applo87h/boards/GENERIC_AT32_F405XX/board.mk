# List of all the board related files.
BOARDSRC = $(BOARD_PATH)/boards/GENERIC_AT32_F405XX/board.c

# Required include directories
BOARDINC = $(BOARD_PATH)/boards/GENERIC_AT32_F405XX

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
