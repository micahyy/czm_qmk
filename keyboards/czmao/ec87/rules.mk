LTO_ENABLE = yes

CUSTOM_MATRIX = lite
SRC += matrix.c

# RAM-only EEPROM (no flash erase, safe with any matrix implementation)
EEPROM_DRIVER = transient

# Debug console output over USB HID
CONSOLE_ENABLE = no

# Use uf2boot 16KB bootloader, 128KB flash on APM32F103CBT6
MCU_LDSCRIPT = STM32F103xB_uf2boot
OPT = 2
