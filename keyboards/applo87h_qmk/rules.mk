# ── MCU / Platform ──────────────────────────────────────────────
# AT32F405RC (LQFP64, 256KB Flash, 102KB SRAM)
# ChibiOS-Contrib AT32F402_405 HAL port
MCU          = AT32F405
MCU_FAMILY   = AT32
MCU_SERIES   = AT32F402_405
MCU_LDSCRIPT = AT32F405xC
MCU_STARTUP  = at32f402
BOARD        = GENERIC_AT32_F405XX
BOOTLOADER   = at32-dfu

# AT32F405 has FPU but QMK's AT32 ChibiOS-Contrib port may not enable it;
# keep software float for maximum compatibility.
USE_FPU      = no

# ── Build ───────────────────────────────────────────────────────
LTO_ENABLE   = yes

# ── QMK Features ────────────────────────────────────────────────
BOOTMAGIC_ENABLE = yes
MOUSEKEY_ENABLE  = yes
EXTRAKEY_ENABLE  = yes
CONSOLE_ENABLE   = no
COMMAND_ENABLE   = no
NKRO_ENABLE      = yes

# ── RGB ─────────────────────────────────────────────────────────
RGBLIGHT_ENABLE  = yes
WS2812_DRIVER    = pwm

# ── Debounce ────────────────────────────────────────────────────
DEBOUNCE_TYPE    = sym_eager_pk

# ── Disable unused features to reduce firmware size ─────────────
SPACE_CADET_ENABLE   = no
GRAVE_ESC_ENABLE     = no
MAGIC_ENABLE         = no
