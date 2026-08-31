# AT32F403ARCT7 target (LQFP64, 256KB Flash, 96KB SRAM, Cortex-M4F @240MHz)
# Pin-to-pin replacement for APM32F103RBT6 / STM32F103RBT6 on the GamerLite board.
# Matrix / WS2812 / USB IDs / features are inherited from the parent keyboard.json.

# 256KB Flash linker script (provided by ChibiOS-Contrib AT32F403_7xx port)
MCU_LDSCRIPT = AT32F403AxC

# AT32 ChibiOS port: enable WFI idle for lower power
OPT_DEFS += -DCORTEX_ENABLE_WFI_IDLE=TRUE

LTO_ENABLE = yes
