# CZMAO Keyboard Firmware

Pre-built firmware for CZMAO keyboards. Download the file matching your keyboard.

## Quick Download

| Keyboard | VIA (Recommended) | Default |
|----------|-------------------|---------|
| **AT87** TKL | [czmao_at87_via.uf2](at87/czmao_at87_via.uf2) | [czmao_at87_default.uf2](at87/czmao_at87_default.uf2) |
| **DS17R6** Numpad | [czmao_ds17r6_via.bin](ds17r6/czmao_ds17r6_via.bin) | [czmao_ds17r6_default.bin](ds17r6/czmao_ds17r6_default.bin) |
| **DS22** Numpad | [czmao_ds22_via.bin](ds22/czmao_ds22_via.bin) | [czmao_ds22_default.bin](ds22/czmao_ds22_default.bin) |
| **GamerLite** | [czmao_gamerlite_via.bin](gamerlite/czmao_gamerlite_via.bin) | [czmao_gamerlite_default.bin](gamerlite/czmao_gamerlite_default.bin) |

## Which version should I use?

- **VIA** (Recommended): Supports live keymap editing via [VIA](https://usevia.app). Flash this if you want to customize your keys without recompiling.
- **Default**: Standard fixed keymap. Flash this if you do not need VIA.

## Flashing Guide

### AT87 (UF2 Drag-and-Drop)

1. Hold the **BOOT** button while plugging in the USB cable (or press the reset button on the PCB).
2. A USB drive named **CRP0DISK** or **CHERRYUF2** will appear.
3. Drag and drop the **.uf2** file onto the drive.
4. The keyboard will automatically restart with the new firmware.

### DS17R6 / DS22 / GamerLite (QMK Toolbox / dfu-util)

Use [QMK Toolbox](https://github.com/qmk/qmk_toolbox/releases) or dfu-util to flash the .bin file. Put your keyboard into bootloader mode by pressing the reset button on the PCB (or hold BOOT while plugging in).

Command-line:
```
dfu-util -d 0483:DF11 -a 0 -s 0x08000000:leave -D your_firmware.bin
```

> **Note for GamerLite users**: This firmware uses a special USB VID/PID for game console adapter compatibility (USB name shows as "HID Keyboard Device"). This is intentional, not a bug.

## VIA Definitions

VIA JSON definition files are available at [via.micah.vip](https://via.micah.vip). After flashing VIA firmware, open [usevia.app](https://usevia.app) and connect your keyboard.

## Support

- Website: [micah.vip](https://micah.vip)
- Issues: Please open a [GitHub Issue](https://github.com/micahyy/qmk/issues)

---

CZMAO (c) 2026. All firmware is based on [QMK](https://qmk.fm).
