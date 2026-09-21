LTO_ENABLE = yes

CUSTOM_MATRIX = lite
SRC += matrix.c ec_calib.c ec_flash.c eeprom_ec87_flash.c

# Flash-backed EEPROM: RAM shadow committed to top flash pages, persistent.
EEPROM_DRIVER = custom
EEPROM_SIZE = 8192

# Debug console output over USB HID
CONSOLE_ENABLE = no

# Use uf2boot 16KB bootloader, 128KB flash on APM32F103CBT6
MCU_LDSCRIPT = STM32F103xB_uf2boot
OPT = 2
