# CZMAO DS22 — VIA Firmware

[English](#english) | [中文](#中文)

---

<a name="english"></a>
## English

Pre-built VIA firmware for CZMAO DS22.

### Files

| File | Size |
|------|------|
| czmao_ds22_via.bin | 34544 bytes |

### Specifications

- MCU: STM32F103
- Layout: 22-key numpad, direct matrix (5x5)
- USB VID:PID: `0x4A17:0x0002`
- RGB: WS2812
- NKRO: Enabled
- LTO: Enabled

### Changelog

| Date | Changes |
|------|---------|
| 2026-08-23 | Added pre-built VIA firmware binary to this folder. |
| 2026-08-22 | Standardized source: VID 0x4A17/PID 0x0002, updated to QMK 2026 API (PAL GPIO, RM_* RGB keycodes), LTO enabled. Merged to main. |

---

<a name="中文"></a>
## 中文

CZMAO DS22 预编译 VIA 固件。

### 文件

| 文件 | 大小 |
|------|------|
| czmao_ds22_via.bin | 34544 字节 |

### 规格

- 主控: STM32F103
- 配列: 22键数字小键盘，直连矩阵（5x5）
- USB VID:PID: `0x4A17:0x0002`
- 灯效: WS2812 RGB
- NKRO: 已启用
- LTO: 已启用（减小固件体积）

### 更新日志

| 日期 | 修改内容 |
|------|---------|
| 2026-08-23 | 在本文件夹中添加预编译 VIA 固件。 |
| 2026-08-22 | 源码标准化：VID 0x4A17/PID 0x0002，升级 QMK 2026 API（PAL GPIO、RM_* RGB 键码名），启用 LTO，合并到 main 分支。 |

---

CZMAO (c) 2026
