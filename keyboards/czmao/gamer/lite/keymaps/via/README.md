# CZMAO GamerLite — VIA Firmware

[English](#english) | [中文](#中文)

---

<a name="english"></a>
## English

Pre-built VIA firmware for CZMAO GamerLite.

### Files

| File | Size |
|------|------|
| czmao_gamerlite_via.bin | 41644 bytes |

### Specifications

- MCU: STM32F103
- Layout: 35-key, direct matrix (6x7)
- USB VID:PID: `0x1A2C:0x50EF`
- USB Device Name: HID Keyboard Device
- RGB: WS2812
- NKRO: Enabled

> **Important:** The VID/PID (`1A2C:50EF`) and USB name ("HID Keyboard Device") are intentionally set for game console adapter (王座) compatibility. Do not modify these values.

### Changelog

| Date | Changes |
|------|---------|
| 2026-08-23 | Added pre-built VIA firmware binary to this folder. |
| 2026-08-22 | Restored original v3 settings after accidental modification. VID/PID/USB name preserved for game console adapter compatibility. Only QMK 2026 compile fixes applied: GPIO API migrated to PAL (palSetLineMode/palClearLine), RGB keycodes renamed to RM_* prefix. Merged to main. |

---

<a name="中文"></a>
## 中文

CZMAO GamerLite 预编译 VIA 固件。

### 文件

| 文件 | 大小 |
|------|------|
| czmao_gamerlite_via.bin | 41644 字节 |

### 规格

- 主控: STM32F103
- 配列: 35键，直连矩阵（6x7）
- USB VID:PID: `0x1A2C:0x50EF`
- USB设备名称: HID Keyboard Device
- 灯效: WS2812 RGB
- NKRO: 已启用

> **重要：** VID/PID（`1A2C:50EF`）和 USB 名称（"HID Keyboard Device"）是为适配王座/手柄转换器刻意设置的，请勿修改。

### 更新日志

| 日期 | 修改内容 |
|------|---------|
| 2026-08-23 | 在本文件夹中添加预编译 VIA 固件。 |
| 2026-08-22 | 恢复原始 v3 设置（此前被误改）。保留王座适配所需的 VID/PID/USB 名称。仅修复 QMK 2026 编译问题：GPIO API 迁移为 PAL（palSetLineMode/palClearLine），RGB 键码名改为 RM_* 前缀。合并到 main 分支。 |

---

CZMAO (c) 2026
