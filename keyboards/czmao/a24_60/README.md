# CZMAO A24 60% (Multy)

60% multi-layout mechanical keyboard with STM32F103 MCU.

CZMAO A24 60% 多配列机械键盘，采用 STM32F103 主控。

* Keyboard Maintainer: [micahyy](https://github.com/micahyy)
* Hardware Supported: CZMAO A24 60% PCB (STM32F103)
* Hardware Availability: CZMAO

## Hardware Specs

| Item | Detail |
|------|--------|
| MCU | STM32F103 |
| Bootloader | STM32duino |
| Key Count | 60% (LAYOUT_60_mutly, 67 positions supporting multiple layouts) |
| Matrix | COL2ROW |
| Caps Lock LED | PA3 |
| USB VID/PID | 0x4A24 / 0x4A24 |
| VIA Layers | 8 |

## Building

```bash
make czmao/a24_60:default
```

## Flashing

Enter STM32duino bootloader mode (hold BOOT0 + press RESET), then:

```bash
make czmao/a24_60:default:flash
```

## VIA

```bash
make czmao/a24_60:via
```

After flashing VIA firmware, open [usevia.app](https://usevia.app) to configure keys.

## Changelog

* 2025-08-25: Code formatting standardized; README added.
