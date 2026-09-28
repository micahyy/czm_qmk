# CZMAO left64

64-key mechanical keyboard with STM32F103 MCU.

CZMAO left64 64键机械键盘，采用 STM32F103 主控。

* Keyboard Maintainer: [micahyy](https://github.com/micahyy)
* Hardware Supported: CZMAO left64 PCB (STM32F103)
* Hardware Availability: CZMAO

## Hardware Specs

| Item | Detail |
|------|--------|
| MCU | STM32F103 |
| Bootloader | STM32duino |
| Key Count | 64 |
| Matrix | 6 rows × 7 cols (COL2ROW) |
| USB VID/PID | 0x7F89 / 0x4A35 |

## Building

```bash
make czmao/left64:default
```

## Flashing

Enter STM32duino bootloader mode (hold BOOT0 + press RESET), then:

```bash
make czmao/left64:default:flash
```

## VIA

```bash
make czmao/left64:via
```

After flashing VIA firmware, open [usevia.app](https://usevia.app) to configure keys.

## Changelog

* 2025-08-25: Code formatting standardized; README added.
