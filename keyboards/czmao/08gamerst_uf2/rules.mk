# Build Options
#   change yes to no to disable

# MCU_LDSCRIPT 		= STM32F103xB

# BOOTMAGIC_ENABLE 	= yes   # Enable Bootmagic Lite
# MOUSEKEY_ENABLE 	= yes	# Mouse keys
# EXTRAKEY_ENABLE 	= yes	# Audio control and System control
# CONSOLE_ENABLE 		= yes	# Console for debug
# COMMAND_ENABLE 		= yes   # Commands for debug and configuration
# NKRO_ENABLE 		= yes   # Enable N-Key Rollover
# BACKLIGHT_ENABLE 	= no
# RGBLIGHT_ENABLE 	= yes
# SLEEP_LED_ENABLE 	= no

# Enter lower-power sleep mode when on the ChibiOS idle thread
# OPT_DEFS 		   += -DCORTEX_ENABLE_WFI_IDLE=TRUE
ENCODER_ENABLE = yes
SRC                += encoder.c

# Host-compat (throne / phone OTG) switches: NKRO + media key gating over VIA.
SRC                += gamerst.c

# WS2812 on B13 = TIM1_CH1N: PWM+DMA instead of bitbang.
# Bitbang uses NOP-timed GPIO toggling that runs from Flash; on APM32F103CBT6
# the flash wait-state behaviour stretches the 0-bit high time past WS2812's
# ~0.5us logic-1 threshold -> every bit reads as 1 -> solid white, and no
# software change (effect/color/brightness) is observable. Hardware PWM+DMA
# decouples the waveform from CPU/Flash timing and is clone-proof.
# Mirrors the proven config in qmk_czmao/keyboards/czmao/ds22/ds22_r6_uf2.
WS2812_DRIVER = pwm