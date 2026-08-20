# SLK Applo87H QMK Firmware

87-key TKL custom keyboard powered by the **Artery AT32F405RC** MCU with
**USB 2.0 High-Speed (480 Mbps)** for **8000 Hz polling rate**.

## Key Specs

| Spec | Value |
|------|-------|
| MCU | Artery AT32F405RC (LQFP64, 216 MHz Cortex-M4F) |
| Flash / RAM | 256 KB / 102 KB |
| USB | OTGHS with on-chip High-Speed PHY (480 Mbps) |
| Polling rate | 8000 Hz (bInterval=1, HS microframe = 125 µs) |
| Layout | ANSI TKL 87-key |
| Switch matrix | 6 rows × 17 cols, COL2ROW |
| RGB | 32× WS2812 (PC6 / TIM3_CH1 PWM+DMA) |
| Indicators | Caps Lock (PA2), Scroll Lock (PD2) |
| Bootloader | AT32-DFU (on-chip USB DFU) |
| VID:PID | 0x534C:0x0871 |

## How 8K Is Achieved

The AT32F405 contains a dedicated **OTGHS controller** with a built-in
**USB 2.0 High-Speed PHY** (480 Mbps). Unlike the STM32F405 which requires
an external ULPI PHY for High-Speed, the AT32F405 routes USB D+/D− directly
to the on-chip HS PHY.

In High-Speed mode, a USB `bInterval` value of **1** equals **one
microframe (125 µs)**, giving a maximum interrupt endpoint rate of
**8000 Hz** — 8× faster than Full-Speed's 1000 Hz.

The enabling chain:

1. **`board.h`** defines `BOARD_OTG2_USES_ULPI` — despite the legacy name,
   on AT32 this macro tells the OTGv1 LLD to select the on-chip HS PHY
   clock/pad path and enable `crmEnableOTG_HS()`.
2. **`configs/mcuconf.h`** sets `AT32_USB_USE_OTG2 TRUE` +
   `AT32_USE_USB_OTG2_HS TRUE`, which configures DCFG for 480 Mbps HS.
3. **`config.h`** sets `USB_POLLING_INTERVAL_MS 1`, so QMK sets
   `bInterval=1` in the HID descriptor.
4. The PLLU generates a precise **48 MHz** USB PHY clock
   (FU=18: 864 MHz ÷ 18 = 48 MHz, within 47.88–48.12 MHz spec).

## Clock Tree

```
HEXT  = 12 MHz (external crystal)
PLL   input  = 12 MHz / MS(1) = 12 MHz
      VCO    = 12 MHz × NS(72) = 864 MHz
      SYSCLK = 864 MHz / FP(4) = 216 MHz
PLLU  = 864 MHz / FU(18) = 48 MHz  → OTGHS PHY
AHB   = 216 MHz
APB1  = 108 MHz (TIM3 = 216 MHz for WS2812)
APB2  = 108 MHz
LDO   = 1.3 V (LEV3, required for 216 MHz)
```

## File Structure

```
applo87h_qmk/
├── keyboard.json          # QMK info metadata (pins, layout, USB, features)
├── rules.mk               # MCU family/series/startup/board/bootloader
├── config.h               # USB polling, matrix, debounce, LED, RGB
├── halconf.h              # ChibiOS HAL driver switches
├── chconf.h               # ChibiOS kernel config (100 kHz tick)
├── README.md
├── boards/
│   └── GENERIC_AT32_F405XX/
│       ├── board/
│       │   ├── board.h    # MCU type, HEXT, GPIO init, BOARD_OTG2_USES_ULPI
│       │   ├── board.c    # __early_init() → clock init; boardInit() disables JTAG
│       │   └── board.mk
│       └── configs/
│           ├── config.h   # VBUS ignore, endpoint reorder, bootloader jump
│           └── mcuconf.h  # Clock tree, OTG2 HS enable, TMR3 PWM, IRQ prios
└── keymaps/
    ├── default/keymap.c   # Base + Fn layer
    └── via/
        ├── keymap.c
        └── rules.mk       # VIA_ENABLE = yes
```

## Pin Map

| Function | Pin |
|----------|-----|
| Rows | PC12, PC11, PC10, PC3, PC2, PC8 |
| Cols | PA15, PA12, PA11, PA10, PA9, PA8, PB15, PB14, PB13, PB12, PB10, PB1, PB0, PA5, PA4, PB9, PB8 |
| USB D+/D− | OTGHS1 dedicated pads |
| WS2812 data | PC6 (TIM3_CH1) |
| Caps Lock LED | PA2 |
| Scroll Lock LED | PD2 |
| Boot button | PF11 |
| Reset button | PF0 |

> **Note:** JTAG is disabled in `boardInit()` to free PA15 (COL0), PB3, and
> PB4 for matrix use. SWD (PA13/PA14) remains available for debugging.

## Building

This firmware targets the QMK `develop` branch which includes ChibiOS-Contrib
with the AT32F402_405 HAL port.

```bash
# From qmk_firmware root:
qmk compile -kb applo87h_qmk -km default

# With VIA:
qmk compile -kb applo87h_qmk -km via
```

The keyboard folder should be placed at:
```
qmk_firmware/keyboards/applo87h_qmk/
```

## Flashing

### Method 1: Hardware DFU (recommended)

1. Hold the **BOOT** button (PF11) on the PCB.
2. While holding BOOT, press and release **RESET** (PF0), then release BOOT.
3. The board enumerates as AT32 DFU device (VID:PID = `2E3C:DF11`).
4. Flash:
   ```bash
   dfu-util -a 0 -d 2E3C:DF11 -s 0x08000000:leave -D applo87h_qmk_default.bin
   ```

### Method 2: Bootmagic

Hold **Esc** while plugging in the USB cable to enter the bootloader.

### Method 3: QMK Toolbox

Use QMK Toolbox with the AT32-DFU bootloader selected, or auto-detect.

## Tuning for 8K

- `USB_POLLING_INTERVAL_MS 1` — sets HID `bInterval=1` (125 µs in HS)
- `DEBOUNCE 1` + `sym_eager_pk` — 1 ms eager debounce, reports immediately
- `CH_CFG_ST_FREQUENCY 100000` — 100 kHz kernel tick (10 µs resolution)
- `MATRIX_IO_DELAY 1` — minimal GPIO settling delay
- `NKRO_ENABLE` — n-key rollover for fast simultaneous presses
