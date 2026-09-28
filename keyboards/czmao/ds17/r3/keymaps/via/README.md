# CZMAO DS17R3 — VIA Firmware

[English](#english) | [中文](#中文)

---

<a name="english"></a>
## English

Pre-built VIA firmware for CZMAO DS17R3 (diode matrix version).

### Files

| File | Size |
|------|------|
| czmao_ds17r3_via.bin | 33680 bytes |

### Specifications

- MCU: **Geehy APM32F103CBT6** (STM32F103 compatible, drop-in replacement)
- Layout: 17-key numpad, 5x4 diode matrix (COL2ROW)
- USB VID:PID: 0x4A17:0x4A18
- RGB: WS2812
- NKRO: Enabled

> DS17R3 and DS17R6 share the same VID/PID and VIA definition.

### 3D Printed Case

https://makerworld.com.cn/zh/@micahyy/upload

### Changelog

| Date | Changes |
|------|---------|
| 2026-08-23 | Initial r3 build. Fixed QMK 2026 compilation: GPIO API migrated to PAL, RGB keycodes renamed to RM_*. All 8 VIA layers. MCU: APM32F103CBT6. |
| 2026-01-11 | Original r3 firmware (diode matrix, COL2ROW scan). |

---

<a name="中文"></a>
## 中文

CZMAO DS17R3（二极管矩阵版）预编译 VIA 固件。

### 文件

| 文件 | 大小 |
|------|------|
| czmao_ds17r3_via.bin | 33680 字节 |

### 规格

- 主控: **极海 APM32F103CBT6**（兼容STM32F103，可直接替换）
- 配列: 17键数字小键盘，5x4二极管矩阵（COL2ROW）
- USB VID:PID: 0x4A17:0x4A18
- 灯效: WS2812 RGB
- NKRO: 已启用

> DS17R3 和 DS17R6 共享相同的 VID/PID 和 VIA 定义。

### 3D打印外壳

https://makerworld.com.cn/zh/@micahyy/upload

### 更新日志

| 日期 | 修改内容 |
|------|---------|
| 2026-08-23 | r3首次编译。修复QMK 2026编译：GPIO API迁移为PAL，RGB键码改为RM_*前缀，补全8个VIA图层。主控：APM32F103CBT6。 |
| 2026-01-11 | 原始r3固件（二极管行列扫描）。 |

---

CZMAO (c) 2026
