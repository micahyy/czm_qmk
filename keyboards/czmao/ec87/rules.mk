LTO_ENABLE = yes

CUSTOM_MATRIX = lite
SRC += matrix.c

CONSOLE_ENABLE = no
MOUSEKEY_ENABLE = no

# 16KB UF2 bootloader, app only 48KB flash
MCU_LDSCRIPT = STM32F103x8_uf2boot
OPT = s
