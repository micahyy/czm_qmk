# SLK Applo87H QMK Firmware

87-key TKL custom keyboard powered by the **Artery AT32F405RCT7-7** MCU with
**USB 2.0 High-Speed (480 Mbps)** for **8000 Hz polling rate**.

## Key Specs

| Spec | Value |
|------|-------|
| MCU | Artery AT32F405RCT7-7 (LQFP64, 216 MHz Cortex-M4F) |
| Flash / RAM | 256 KB / 96 KB |
| USB | OTGHS with on-chip High-Speed PHY (480 Mbps) |
| Polling rate | 8000 Hz (bInterval=1, HS microframe = 125 µs) |
| Layout | ANSI TKL 87-key |
| Switch matrix | 6 rows × 17 cols, COL2ROW |
| Indicators | Caps Lock (PA2), Scroll Lock (PD2) |
| Bootloader | AT32-DFU (on-chip USB DFU) |
| VID:PID | 0x534C:0x0871 |

## Pin Map

| Function | Pins |
|----------|------|
| Rows (0-5) | PC12, PC11, PC10, PC5, PC3, PC2 |
| Cols (0-16) | PA15, PA12, PA11, PA10, PA9, PA8, PB12, PB10, PB2, PB1, PB0, PA7, PA6, PA5, PA4, PA3, PB9 |
| USB D+/D− | OTGHS1 dedicated pads (Pin 34/35) |
| USB VBUS | PB13 (board pull-up) |
| HEXT crystal | PF0 / PF1 (12 MHz) |
| LEXT crystal | PC14 / PC15 (32.768 kHz) |
| Caps Lock LED | PA2 |
| Scroll Lock LED | PD2 |
| SWD | PA13 (SWDIO), PA14 (SWCLK) |
| BOOT0 | PF11 |

## How 8K Is Achieved

1. **`board.h`** defines `BOARD_OTG2_USES_ULPI` — selects on-chip HS PHY.
2. **`mcuconf.h`** sets `AT32_USB_USE_OTG2 TRUE` + `AT32_USE_USB_OTG2_HS TRUE`.
3. **`config.h`** sets `USB_POLLING_INTERVAL_MS 1` → bInterval=1 = 125µs = 8000Hz.
4. PLLU generates 48 MHz USB PHY clock (FU=18: 864 MHz ÷ 18).

## Building

```bash
qmk compile -kb slk/applo87h -km default
qmk compile -kb slk/applo87h -km via
```

## Flashing

Hold **Esc** while plugging in USB to enter DFU bootloader (Bootmagic),
or hold BOOT0 + tap RESET. Then flash via dfu-util or QMK Toolbox.
