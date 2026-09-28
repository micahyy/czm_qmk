# CZMAO 3000 (G80-3000)

108-key full-size mechanical keyboard with STM32F103 MCU and per-key RGB.

CZMAO 3000（G80-3000）108键全尺寸机械键盘，采用 STM32F103 主控，支持逐键 RGB。

* Keyboard Maintainer: [micahyy](https://github.com/micahyy)
* Hardware Supported: CZMAO 3000 PCB (STM32F103, WS2812 RGB)
* Hardware Availability: CZMAO

## Hardware Specs

| Item | Detail |
|------|--------|
| MCU | STM32F103 |
| Bootloader | STM32duino |
| Key Count | 108 (LAYOUT_108, 112 positions including ISO extras) |
| Matrix | 8 rows × 16 cols (COL2ROW) |
| RGB | WS2812 × 121 per-key |
| USB VID/PID | 0x7F89 / 0x4A36 |

## Building

```bash
make czmao/3000:default
```

## Flashing

Enter STM32duino bootloader mode (hold BOOT0 + press RESET), then:

```bash
make czmao/3000:default:flash
```

## VIA

```bash
make czmao/3000:via
```

After flashing VIA firmware, open [usevia.app](https://usevia.app) to configure keys.

## Changelog

* 2025-08-25: Updated RGB keycodes for newer QMK (`RGB_TOG` → `RM_TOGG`, `RGB_MOD` → `RM_NEXT`); code formatting standardized; README added.
