# Board-level overrides for AT32F405 (applied after mcu_selection.mk).
# QMK's mcu_selection only knows AT32F415, but the actual chip is AT32F405.
# We override MCU_SERIES to use the AT32F402_405 HAL port which correctly
# registers OTG2/HS support for the AT32F405 high-speed USB peripheral.

MCU_SERIES   = AT32F402_405
MCU_STARTUP  = at32f405
MCU_LDSCRIPT = AT32F405xC
