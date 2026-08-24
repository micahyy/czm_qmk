LTO_ENABLE = yes

CUSTOM_MATRIX = lite
SRC += matrix.c

CONSOLE_ENABLE = yes

# Use 16KB UF2 bootloader (uf2boot) instead of 8KB stm32duino
MCU_LDSCRIPT = STM32F103x8_uf2boot
OPT = 2
