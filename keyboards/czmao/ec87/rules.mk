LTO_ENABLE = yes

CUSTOM_MATRIX = lite
SRC += matrix.c

CONSOLE_ENABLE = yes

# Use EC87-specific linker (reserves last 1KB flash for calib data)
MCU_LDSCRIPT = STM32F103x8_ec87
OPT = 2
