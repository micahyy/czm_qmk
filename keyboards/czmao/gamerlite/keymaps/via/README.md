# VIA Firmware / VIA固件

CZMAO GamerLite — VIA keymap firmware

## Files / 文件

| File | Size |
|------|------|
| czmao_gamerlite_via.bin | 41644 bytes |

## Specs / 规格

- MCU: STM32F103
- Layout: 35-key, direct matrix (6x7)
- VID:PID: 0x1A2C:0x50EF (game console adapter compatible)
- USB Name: HID Keyboard Device

> **Note:** The VID/PID and USB name are intentionally set for game console adapter (王座) compatibility. Do not change.

## Changelog / 更新日志

| Date | Changes |
|------|---------|
| 2026-08-22 | Restored to original v3 settings after accidental modification. VID/PID/USB name preserved for adapter compatibility. Only QMK 2026 compile fixes applied (PAL GPIO, RM_* keycodes). |
| 2026-08-23 | Added pre-built VIA firmware binary to this folder. |

---
CZMAO (c) 2026
