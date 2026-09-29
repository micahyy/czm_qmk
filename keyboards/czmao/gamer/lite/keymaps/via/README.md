# CZMAO GamerLite — VIA Firmware

[English](#english) | [中文](#中文)

---

<a name="english"></a>
## English

Pre-built VIA firmware for CZMAO GamerLite.

### Files

| File | Size |
|------|------|
| [../uf2/czmao_gamer_lite_at32f403a_uf2.uf2](../uf2/czmao_gamer_lite_at32f403a_uf2.uf2) | 69632 bytes (v1.1.0, drag-and-drop UF2) |

### Specifications

- MCU: STM32F103
- Layout: 35-key, direct matrix (6x7)
- USB VID:PID: `0x4A17:0x0005`
- USB Device Name: HID Keyboard Device
- RGB: WS2812
- NKRO: Off by default (enable in VIA → Keyboard tab)
- Mouse keys / media keys: Off by default (enable in VIA → Keyboard tab)

> **Important:** The USB name ("HID Keyboard Device") is intentionally set for game console adapter (王座) compatibility. Do not modify it.
> Out of the box only 6KRO keyboard reports are sent, so consoles, phone thrones and phone OTG hosts accept the board. PC features are switched on from the VIA "Keyboard" tab and stored in EEPROM.
> VIA definition: `via/czmao_gamer_lite_v11.json` (load manually in VIA).

### Changelog

| Date | Changes |
|------|---------|
| 2026-09-29 | v1.1.0: new USB ID `0x4A17:0x0005`; keyboard is a plain boot-protocol device again (no Report ID, shared endpoint removed); consoles/NKRO/mouse/media moved to VIA "Keyboard" tab switches, all off by default; debug console and magic keycodes removed; new VIA definition `czmao_gamer_lite_v11.json`. |
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
- USB VID:PID: `0x4A17:0x0005`
- USB设备名称: HID Keyboard Device
- 灯效: WS2812 RGB
- NKRO: 默认关闭（在 VIA → Keyboard 页打开）
- 鼠标键 / 媒体键: 默认关闭（在 VIA → Keyboard 页打开）

> **重要：** USB 名称（"HID Keyboard Device"）是为适配王座/手柄转换器刻意设置的，请勿修改。
> 出厂只发送 6KRO 键盘报文，主机、王座、手机 OTG 均可识别；电脑所需功能在 VIA "Keyboard" 页打开并存入 EEPROM。
> VIA 定义文件：`via/czmao_gamer_lite_v11.json`（在 VIA 里手动加载）。

### 更新日志

| 日期 | 修改内容 |
|------|---------|
| 2026-08-23 | 在本文件夹中添加预编译 VIA 固件。 |
| 2026-08-22 | 恢复原始 v3 设置（此前被误改）。保留王座适配所需的 VID/PID/USB 名称。仅修复 QMK 2026 编译问题：GPIO API 迁移为 PAL（palSetLineMode/palClearLine），RGB 键码名改为 RM_* 前缀。合并到 main 分支。 |

---

CZMAO (c) 2026
