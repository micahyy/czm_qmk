# CZMAO Keyboard Firmware

This repository contains QMK firmware for **CZMAO** custom keyboards.

> **Looking for pre-built firmware?** Check the [**firmware/ folder**](firmware/) for direct downloads and flashing instructions.

## Supported Keyboards

| Keyboard | Description | VIA | Source |
|----------|-------------|-----|--------|
| [CZMAO AT87](keyboards/czmao/czmao_at87/) | 87-key TKL, AT32F405, UF2 bootloader | Yes | [Source](keyboards/czmao/czmao_at87/) |
| [CZMAO DS17R6](keyboards/czmao/ds17r6/) | 17-key numpad, direct matrix | Yes | [Source](keyboards/czmao/ds17r6/) |
| [CZMAO DS22](keyboards/czmao/ds22/) | 22-key numpad, direct matrix | Yes | [Source](keyboards/czmao/ds22/) |
| [CZMAO GamerLite](keyboards/czmao/gamerlite/) | Gaming keyboard (game console adapter compatible) | Yes | [Source](keyboards/czmao/gamerlite/) |

## Quick Start

1. Go to the [**firmware/ folder**](firmware/)
2. Download the firmware for your keyboard
3. Follow the [flashing guide](firmware/README.md#flashing-guide)

## Building from Source

```bash
# Example: build CZMAO DS22 VIA firmware
make czmao/ds22:via COLOR=false

# Example: build CZMAO AT87 VIA firmware
make czmao/czmao_at87:via COLOR=false
```

## About QMK

This is a fork of [QMK Firmware](https://github.com/qmk/qmk_firmware). For full QMK documentation, visit [docs.qmk.fm](https://docs.qmk.fm).

---

CZMAO (c) 2026. Firmware released under GPL v2. Website: [micah.vip](https://micah.vip)
