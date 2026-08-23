# CZMAO DS17R6 — VIA Firmware

[English](#english) | [中文](#中文)

---

<a name="english"></a>
## English

Pre-built VIA firmware for CZMAO DS17R6 (direct pin version).

### Files

| File | Size |
|------|------|
| czmao_ds17r6_via.bin | 33612 bytes |

### Specifications

- MCU: STM32F103
- Layout: 17-key numpad, direct pin matrix (no diodes)
- USB VID:PID: 0x4A17:0x4A18
- RGB: WS2812
- NKRO: Enabled

> DS17R3 and DS17R6 share the same VID/PID and VIA definition. The version jump from r3 to r6 is due to the case revision; no firmware versions were skipped.

### 3D Printed Case

https://makerworld.com.cn/zh/@micahyy/upload

### Changelog

| Date | Changes |
|------|---------|
| 2026-08-23 | Restructured to keyboards/czmao/ds17/r6/. Added r3 alongside. Bilingual README with case link. |
| 2026-08-21 | Per QMK PR review: removed redundant files, formatted keyboard.json, fixed readme. CI passed. |
| 2026-08-20 | Initial r6 release, direct pin matrix. Submitted QMK PR #26412. |

---

<a name="中文"></a>
## 中文

CZMAO DS17R6（直连矩阵版）预编译 VIA 固件。

### 文件

| 文件 | 大小 |
|------|------|
| czmao_ds17r6_via.bin | 33612 字节 |

### 规格

- 主控: STM32F103
- 配列: 17键数字小键盘，直连矩阵（无需二极管）
- USB VID:PID: 0x4A17:0x4A18
- 灯效: WS2812 RGB
- NKRO: 已启用

> DS17R3 和 DS17R6 共享相同的 VID/PID 和 VIA 定义。版本号从r3跳到r6是因为外壳更新到r6，固件版本没有跳过。

### 3D打印外壳

https://makerworld.com.cn/zh/@micahyy/upload

### 更新日志

| 日期 | 修改内容 |
|------|---------|
| 2026-08-23 | 目录重组为 keyboards/czmao/ds17/r6/，同时添加r3。中英文README，含外壳链接。 |
| 2026-08-21 | 根据QMK PR review修改：删除冗余文件、格式化keyboard.json、修复readme，CI通过。 |
| 2026-08-20 | r6首次发布，直连矩阵，提交QMK PR #26412。 |

---

CZMAO (c) 2026
