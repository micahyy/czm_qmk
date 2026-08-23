# CZMAO Gamer V2 - VIA Firmware

## Files / 文件

| File | Description |
|------|-------------|
| `czmao_gamer_v2_via.bin` | Pre-compiled VIA firmware / 预编译VIA固件 |
| `1997619976.json` | VIA JSON definition (VID 0x4E08 / PID 0x4E08) |
| `keymap.c` | VIA keymap source / VIA键位源码 |
| `rules.mk` | VIA build rules / VIA编译规则 |

## Specifications / 规格

| Parameter | Value |
|-----------|-------|
| MCU | Geehy APM32F103CBT6 (STM32F103 compatible) |
| Layout | 37 keys + 1 rotary encoder |
| Matrix | 6x7, diode COL2ROW |
| RGB | WS2812, 34 LEDs |
| USB | VID 0x4E08 / PID 0x4E08 |
| VIA Layers | 8 |

## Changelog / 更新日志

| Date | Changes |
|------|---------|
| 2026-08-23 | Migrated to QMK 2026: keyboard.json, RM_* keycodes, PAL GPIO API, fixed VIA remapping (disabled console/command to free USB endpoints), 8 VIA layers, added VIA JSON |
| 2026-08-23 | 迁移至QMK 2026：keyboard.json、RM_*键码、PAL GPIO API、修复VIA无法改键（关闭console/command释放USB端点）、8层VIA、添加VIA JSON |

## Flashing / 刷机

Hold ESC while plugging in USB, then flash `czmao_gamer_v2_via.bin`.
按住ESC插入USB，然后刷入 `czmao_gamer_v2_via.bin`。

## 3D Printed Case / 3D打印外壳

https://makerworld.com.cn/zh/@micahyy/upload
