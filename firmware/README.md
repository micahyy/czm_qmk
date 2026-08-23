# CZMAO Firmware Downloads / 固件下载

[English](#english) | [中文](#中文)

---

<a name="english"></a>
## English

Pre-built firmware for all CZMAO keyboards.

### Which version should I download?

- **VIA** (Recommended): Supports live keymap editing via [VIA](https://usevia.app). Flash this if you want to customize keys without recompiling.
- **Default**: Standard fixed keymap. Flash this if you do not need VIA.

### Download Links

| Keyboard | VIA (Recommended) | Default |
|----------|-------------------|---------|
| **AT87** TKL | [czmao_at87_via.uf2](at87/czmao_at87_via.uf2) | [czmao_at87_default.uf2](at87/czmao_at87_default.uf2) |
| **DS17R6** Numpad | [czmao_ds17r6_via.bin](ds17r6/czmao_ds17r6_via.bin) | [czmao_ds17r6_default.bin](ds17r6/czmao_ds17r6_default.bin) |
| **DS22** Numpad | [czmao_ds22_via.bin](ds22/czmao_ds22_via.bin) | [czmao_ds22_default.bin](ds22/czmao_ds22_default.bin) |
| **GamerLite** | [czmao_gamerlite_via.bin](gamerlite/czmao_gamerlite_via.bin) | [czmao_gamerlite_default.bin](gamerlite/czmao_gamerlite_default.bin) |

### Flashing Guide

#### AT87 — UF2 (Drag and Drop)

1. Hold the **BOOT** button on the PCB while plugging in the USB cable.
2. A USB drive named **CHERRYUF2** will appear on your computer.
3. Drag and drop the `.uf2` file onto the drive.
4. The keyboard will automatically restart. Done!

> No additional software required.

#### DS17R6 / DS22 / GamerLite — QMK Toolbox or dfu-util

1. Download and install [QMK Toolbox](https://github.com/qmk/qmk_toolbox/releases) (GUI) or use `dfu-util` (command line).
2. Put the keyboard into bootloader mode:
   - Press the **reset button** on the PCB, **or**
   - Hold **BOOT** while plugging in the USB cable.
3. In QMK Toolbox, select the `.bin` file and click **Flash**.

**Command line (dfu-util):**
```
dfu-util -d 0483:DF11 -a 0 -s 0x08000000:leave -D czmao_ds22_via.bin
```

### Note for GamerLite Users

GamerLite uses a special USB VID/PID (`1A2C:50EF`) and shows as **"HID Keyboard Device"** on your computer. This is **intentional** for game console adapter (王座) compatibility. It is not a bug.

### VIA Setup

After flashing VIA firmware:
1. Open [usevia.app](https://usevia.app) in a Chrome/Edge browser.
2. Click **Authorize device** and select your keyboard.
3. Start customizing your keymap.

VIA JSON definitions are hosted at [via.micah.vip](https://via.micah.vip).

---

<a name="中文"></a>
## 中文

所有 CZMAO 键盘的预编译固件。

### 应该下载哪个版本？

- **VIA 版**（推荐）：支持通过 [VIA](https://usevia.app) 在线改键，即改即用，无需重新编译。
- **Default 版**：标准固定键位，不需要改键功能时刷入此版本。

### 下载链接

| 键盘 | VIA 版（推荐） | Default 版 |
|------|---------------|------------|
| **AT87** TKL | [czmao_at87_via.uf2](at87/czmao_at87_via.uf2) | [czmao_at87_default.uf2](at87/czmao_at87_default.uf2) |
| **DS17R6** 数字小键盘 | [czmao_ds17r6_via.bin](ds17r6/czmao_ds17r6_via.bin) | [czmao_ds17r6_default.bin](ds17r6/czmao_ds17r6_default.bin) |
| **DS22** 数字小键盘 | [czmao_ds22_via.bin](ds22/czmao_ds22_via.bin) | [czmao_ds22_default.bin](ds22/czmao_ds22_default.bin) |
| **GamerLite** | [czmao_gamerlite_via.bin](gamerlite/czmao_gamerlite_via.bin) | [czmao_gamerlite_default.bin](gamerlite/czmao_gamerlite_default.bin) |

### 刷写教程

#### AT87 — UF2 拖拽刷写

1. 按住 PCB 上的 **BOOT** 键，同时插入 USB 线。
2. 电脑上会出现一个名为 **CHERRYUF2** 的 U 盘。
3. 将 `.uf2` 文件直接拖入该 U 盘。
4. 键盘自动重启，刷写完成！

> 无需安装任何软件。

#### DS17R6 / DS22 / GamerLite — QMK Toolbox 或 dfu-util

1. 下载安装 [QMK Toolbox](https://github.com/qmk/qmk_toolbox/releases)（图形界面），或使用命令行工具 `dfu-util`。
2. 让键盘进入刷写模式：
   - 按一下 PCB 上的 **reset 按钮**，**或**
   - 按住 **BOOT** 键的同时插入 USB 线。
3. 在 QMK Toolbox 中选择 `.bin` 文件，点击 **Flash**。

**命令行（dfu-util）：**
```
dfu-util -d 0483:DF11 -a 0 -s 0x08000000:leave -D czmao_ds22_via.bin
```

### GamerLite 用户须知

GamerLite 使用特殊的 USB VID/PID（`1A2C:50EF`），在电脑上显示为 **"HID Keyboard Device"**。这是**刻意设计**的，用于兼容王座/手柄转换器，不是固件问题，请勿修改。

### VIA 使用方法

刷入 VIA 版固件后：
1. 使用 Chrome 或 Edge 浏览器打开 [usevia.app](https://usevia.app)。
2. 点击 **Authorize device**（授权设备），选择你的键盘。
3. 即可在线修改键位。

VIA JSON 定义文件托管在 [via.micah.vip](https://via.micah.vip)。

---

### Changelog / 更新日志

| Date 日期 | Changes / 修改内容 |
|-----------|-------------------|
| 2026-08-23 | Initial firmware release folder. AT87 (UF2+BIN), DS17R6, DS22, GamerLite — both VIA and Default builds.<br>初始固件发布。AT87（UF2+BIN格式）、DS17R6、DS22、GamerLite，均包含 VIA 版和 Default 版。 |

---

CZMAO &copy; 2026. Firmware based on [QMK](https://qmk.fm). Website: [micah.vip](https://micah.vip)
