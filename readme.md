# CZMAO Keyboard Firmware

[English](#english) | [中文](#中文)

---

<a name="english"></a>
## English

Pre-built QMK firmware for **CZMAO** custom keyboards.

> **[Download firmware here → firmware/ folder](firmware/)**

### Supported Keyboards

| Keyboard | Description | VIA | Source |
|----------|-------------|-----|--------|
| **CZMAO AT87** | 87-key TKL, AT32F405 MCU, UF2 drag-and-drop | ✅ | [Source](keyboards/czmao/czmao_at87/) |
| **CZMAO DS17R6** | 17-key numpad, STM32F103, direct matrix | ✅ | [Source](keyboards/czmao/ds17r6/) |
| **CZMAO DS22** | 22-key numpad, STM32F103, direct matrix | ✅ | [Source](keyboards/czmao/ds22/) |
| **CZMAO GamerLite** | Gaming keyboard (game console adapter compatible) | ✅ | [Source](keyboards/czmao/gamerlite/) |

### Quick Start

1. Go to the [**firmware/ folder**](firmware/)
2. Download the firmware for your keyboard
3. Follow the [flashing instructions](firmware/README.md)

### Building from Source

```bash
# Build VIA firmware (recommended)
make czmao/ds22:via COLOR=false
make czmao/ds17r6:via COLOR=false
make czmao/gamerlite:via COLOR=false
make czmao/czmao_at87:via COLOR=false
```

### Changelog

| Date | Changes |
|------|---------|
| 2026-08-23 | Added `firmware/` folder with pre-built binaries and bilingual flashing guide. Updated root README. |
| 2026-08-22 | DS22 source standardized and merged to main. GamerLite v3 restored (VID/PID preserved for game console adapter compatibility). |
| 2026-08-21 | AT87 firmware added with CherryUF2 bootloader support. DS17R6 PR submitted to QMK upstream (#26412). |
| 2026-08-20 | Initial CZMAO keyboard source files added (AT87, DS17R6, DS22, GamerLite). |

---

<a name="中文"></a>
## 中文

**CZMAO（菜籽猫）**客制化键盘 QMK 固件。

> **[点击下载固件 → firmware/ 文件夹](firmware/)**

### 支持的键盘

| 键盘 | 说明 | VIA | 源码 |
|------|------|-----|------|
| **CZMAO AT87** | 87键TKL，AT32F405主控，UF2拖拽刷入 | ✅ | [源码](keyboards/czmao/czmao_at87/) |
| **CZMAO DS17R6** | 17键数字小键盘，STM32F103，直连矩阵 | ✅ | [源码](keyboards/czmao/ds17r6/) |
| **CZMAO DS22** | 22键数字小键盘，STM32F103，直连矩阵 | ✅ | [源码](keyboards/czmao/ds22/) |
| **CZMAO GamerLite** | 游戏键盘（适配王座/手柄转换器） | ✅ | [源码](keyboards/czmao/gamerlite/) |

### 快速开始

1. 进入 [**firmware/ 文件夹**](firmware/)
2. 下载对应键盘的固件
3. 按照 [刷写教程](firmware/README.md) 操作

### 自行编译

```bash
# 编译 VIA 版本固件（推荐）
make czmao/ds22:via COLOR=false
make czmao/ds17r6:via COLOR=false
make czmao/gamerlite:via COLOR=false
make czmao/czmao_at87:via COLOR=false
```

### 更新日志

| 日期 | 修改内容 |
|------|---------|
| 2026-08-23 | 新增 `firmware/` 文件夹，包含预编译固件和中英文刷写指南；更新根目录 README。 |
| 2026-08-22 | DS22 源码标准化并合并到 main 分支；GamerLite v3 恢复（保留王座适配的 VID/PID）。 |
| 2026-08-21 | 新增 AT87 固件，支持 CherryUF2 bootloader；DS17R6 提交 QMK 官方 PR（#26412）。 |
| 2026-08-20 | 初始提交 CZMAO 键盘源码（AT87、DS17R6、DS22、GamerLite）。 |

---

CZMAO &copy; 2026. Based on [QMK Firmware](https://qmk.fm). Website: [micah.vip](https://micah.vip)
