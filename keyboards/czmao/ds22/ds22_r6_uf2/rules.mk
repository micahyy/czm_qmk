# Use 128KB flash (APM32F103CBT6) with 16KB UF2 bootloader (CherryUF2)
MCU_LDSCRIPT = STM32F103xB_uf2boot

# BOOTLOADER=custom injects platforms/chibios/bootloaders/custom.c, which only
# provides weak stubs, so the keyboard-level bootloader.c override below wins
# (BOOTLOADER=uf2boot would inject a strong uf2boot.c and clash with it).
# BOARD is pinned to the platform board used by the known-good r6 / ds17r6_uf2
# builds (stm32duino / uf2boot both force BOARD=STM32_F103_STM32DUINO in
# bootloader.mk); keeping it here guarantees the same proven board chain even
# with BOOTLOADER=custom, which would otherwise fall back to a keyboard-local
# board copy that made RGB flicker on hardware.
BOOTLOADER = custom
BOARD = STM32_F103_STM32DUINO
FIRMWARE_FORMAT = uf2

SRC += bootloader.c

# WS2812 on B13 = TIM1_CH1N: PWM+DMA instead of bitbang
WS2812_DRIVER = pwm
