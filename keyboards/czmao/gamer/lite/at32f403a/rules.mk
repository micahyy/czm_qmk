# AT32F403ARCT7 target (LQFP64, 256KB Flash, 96KB SRAM, Cortex-M4F @240MHz)
# Pin-to-pin compatible with STM32F103RBT6 / APM32F103RBT6.
MCU_LDSCRIPT = AT32F403AxC

# AT32 ChibiOS port: enable WFI idle for power saving
OPT_DEFS += -DCORTEX_ENABLE_WFI_IDLE=TRUE

# Keep LTO as per the shipping keymap rules
LTO_ENABLE = yes
