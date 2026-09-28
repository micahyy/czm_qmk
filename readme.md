# CZMAO Keyboard Firmware

[English](#english) | [中文](#中文)

---

<a name="english"></a>
## English

QMK firmware for **CZMAO** custom keyboards.

### Keyboards

| Keyboard | Description | Source |
|----------|-------------|--------|
| **CZMAO AT87** | 87-key TKL, AT32F405, UF2 | [keyboards/czmao/czmao_at87/](keyboards/czmao/czmao_at87/) |
| **CZMAO DS17R6** | 17-key numpad | [keyboards/czmao/ds17r6/](keyboards/czmao/ds17r6/) |
| **CZMAO DS22** | 22-key numpad | [keyboards/czmao/ds22/](keyboards/czmao/ds22/) |
| **CZMAO GamerLite** | Gaming (adapter compatible) | [keyboards/czmao/gamerlite/](keyboards/czmao/gamerlite/) |

Pre-built VIA firmware is located in each keyboard's `keymaps/via/` folder.

### Build

```bash
make czmao/ds22:via COLOR=false
```

### Changelog

| Date | Changes |
|------|---------|
| 2026-08-23 | Added VIA firmware binaries to each keyboard's `keymaps/via/` folder. Bilingual README. |
| 2026-08-22 | DS22 and GamerLite source merged to main. |
| 2026-08-21 | AT87 added. DS17R6 PR submitted upstream (#26412). |
| 2026-08-20 | Initial CZMAO keyboard sources. |

---

<a name="中文"></a>
## 中文

**CZMAO（菜籽猫）**客制化键盘 QMK 固件。

### 键盘列表

| 键盘 | 说明 | 源码 |
|------|------|------|
| **CZMAO AT87** | 87键TKL，AT32F405，UF2 | [keyboards/czmao/czmao_at87/](keyboards/czmao/czmao_at87/) |
| **CZMAO DS17R6** | 17键数字小键盘 | [keyboards/czmao/ds17r6/](keyboards/czmao/ds17r6/) |
| **CZMAO DS22** | 22键数字小键盘 | [keyboards/czmao/ds22/](keyboards/czmao/ds22/) |
| **CZMAO GamerLite** | 游戏键盘（适配王座） | [keyboards/czmao/gamerlite/](keyboards/czmao/gamerlite/) |

预编译的 VIA 固件在每个键盘的 `keymaps/via/` 文件夹内。

### 编译

```bash
make czmao/ds22:via COLOR=false
```

### 更新日志

| 日期 | 修改内容 |
|------|---------|
| 2026-08-23 | 在每个键盘的 `keymaps/via/` 文件夹中放入预编译 VIA 固件；中英文 README。 |
| 2026-08-22 | DS22 和 GamerLite 源码合并到 main 分支。 |
| 2026-08-21 | 新增 AT87；DS17R6 提交 QMK 官方 PR（#26412）。 |
| 2026-08-20 | 初始提交 CZMAO 键盘源码。 |

---

CZMAO (c) 2026. Based on [QMK](https://qmk.fm). [micah.vip](https://micah.vip)
