# CZMAO 08 Gamer ST (UF2 variant)

左手 35 键游戏键盘 + 1 旋钮。MCU 是 STM32F103CBT6 或脚位兼容的国产
APM32F103CBT6；固件走 UF2（CherryUF2）拖放烧录。

## Bootloader

UF2 拖入即刷。三种进入方式：

- **Bootmagic**：按住 matrix (0,0) 那颗键的同时插 USB
- **板载 RESET**：双击 PCB 上的 RESET 键
- **QK_BOOT**：VIA 里分配 QK_BOOT 键（写 CherryUF2 的双击魔数 `0xf01669ef` 后复位）

首次刷入 bootloader：用 ST-Link 把 `uf2boot_czm_apm32f103.hex` 烧到
`0x08000000`，App 起始 `0x08004000`，UF2 family id `0x5ee21072`。

## 驱动页升级（没有一键升级）

CherryUF2 只有一个 USB 大容量存储盘，**没有任何可写的 HID 接口**
（扫过 `uf2boot_czm_apm32f103.hex`：无 vendor usage page `0xFF60`）。
浏览器写不了 U 盘，所以本板不可能做 DC87 那种一键刷写。

- `configs.json` 里**不要**给 `0x4E08:0x4E0A` 写 `firmwareTransport: "hid"`，
  写了驱动页就会去等一个不存在的 HID 设备，表现是“看得到新版本但 update 不动”
- 驱动页走下载 + 进 bootloader + 拖文件那条流程
- “进 bootloader”按钮发的是 VIA 命令 `0x0B`。本仓库的 `quantum/via.c`
  原本没有这个 case（回 `id_unhandled`，按钮点了没反应），已补上
  `reset_keyboard()`。**1.0.5 及以后才响**；更早的固件请用板载 RESET 双击
  或 Bootmagic
- 真要一键升级只能重做 bootloader，已出厂的板子救不回来

## USB 识别

| | App | Bootloader |
|---|---|---|
| VID | `0x4E08` | `0x4E08` |
| PID | `0x4E0A` | `0x4E0B` |
| 产品名字符串 | `CZMAO Gamer ST` | 由 CherryUF2 决定 |

产品名走自有牌子（`config.h` 的 `#define PRODUCT`）。GamerLite 为了过
王座那关伪装成了 `HID Keyboard Device`，但王座到底按不按产品名匹配
没有公开资料也没实测确认。如果实机插上王座不识别，第一步就把这行
改回 `HID Keyboard Device` 再试；再不行才考虑换成 Lite 那组已经验证过
能过王座的 VID/PID `0x4A17:0x0005`（会同时影响驱动页匹配，慎用）。

## 王座 / 手机外接兼容

`gamerst.c` 提供主机兼容门控，出厂状态：

- **6KRO boot protocol**：`force_nkro` 已去掉，全键无冲默认关
- **鼠标键编译期关闭**：`mousekey: false`。描述符里只要挂 Mouse collection，
  王座/安卓就把整把键绑到鼠标通路，表现是“按键变成光标移动”。
  这一项不能只靠运行时开关，必须编译期砍掉
- **多媒体 / 系统键默认开**：旋钮音量开箱可用。Consumer 和 System
  各占独立 report id，不会污染键盘报文；万一某个主机闹脾气，VIA 里可关
- **console / command 关闭**：不占额外接口，手机侧枚举更干净
- **`USB_SUSPEND_WAKEUP_DELAY 500`**：慢速 hub 重新挂载用
- **出厂亮度 100**（上限仍是 `max_brightness: 180`）：王座的 USB-A 口
  和鼠标分电流，35 颗 WS2812 全白容易把口拉崩

VIA “Keyboard” 页的开关（写 EEPROM，掉电保留）：

| value id | 作用 | 出厂 |
|---|---|---|
| 1 | 恢复默认键位 | 按钮 |
| 2 | 进入 bootloader | 按钮 |
| 3 | NKRO | 关 |
| 5 | 多媒体 / 系统键 | 开 |

value id 4 是 GamerLite 的鼠标键开关，本板保留空洞不复用，方便两边对表。

改完这几项需要重刷，老用户 EEPROM 里没有新的 magic 值，首次启动会自动
写成出厂默认，不会继承旧的 NKRO 状态。

## WS2812

单颗 per-key，数据线 **B13**。走 `WS2812_DRIVER = pwm`：TIM1_CH1N
（complementary output + AFIO full remap）+ DMA1 Stream 5 = TIM1_UP。

不要退回 bit-bang：APM32F103CBT6 的 flash 取指等待会把 0-bit 高电平
拉到超过 WS2812 的 ~0.5 us 判决门限，结果每一位都被识别成 1，
整块矩阵常亮纯白、VIA 也调不动。这个配置和
`keyboards/czmao/ds22/ds22_r6_uf2` 保持一致。

`halconf.h` / `mcuconf.h` 里有键盘级 override，因为 czm_qmk 里
`STM32_F103_STM32DUINO` 默认 `STM32_PWM_USE_TIM1 = FALSE`、
`HAL_USE_PWM = FALSE`，不开就编不出 PWM 链。

## RGB 指示层

`keymaps/uf2/keymap.c` 里的 `rgb_matrix_indicators_advanced_user()`
只在动效渲染之后覆盖两颗 LED：

- **LED 18**（CAPS，matrix[3,0]）：Caps Lock 打开时叠白
- **LED 31**（FN/MO(1)，matrix[5,1]）：按当前最高层显示单色，
  1 红 / 2 绿 / 3 蓝 / 4 粉 / 5 黄 / 6 紫 / 7 白

不要在这个函数里对 `[led_min, led_max)` 整段重涂，会把 rgb_matrix 的
所有动效盖掉、VIA 灯效控制看起来全部失灵。

## 快捷键

| 按键 | 触发 | 备注 |
|---|---|---|
| FN+ALT+Lsft | 打开/关闭 RGB | |
| FN+ALT+Z | 切换灯效 | |
| FN+ALT+S / X | 饱和度 +/- | |
| FN+ALT+D / C | 色相 +/- | |
| FN+ALT+F / V | 亮度 +/- | |
| FN+ALT+G / B | 灯效速度 +/- | |
| FN+ALT+Tab | 全键无冲 开/关 | 默认关（6KRO） |
| FN+ALT+ESC | 恢复出厂 | |
| FN+ALT+Mute | 进入 bootloader | Mute = 旋钮按下 或 F5 |

## 旋钮

A5 / A6，`ENCODERS_MATRIX_MAP` 映射到 row 5 的 VOLU / VOLD 两个虚拟位，
可以在每一层（含 VIA）单独指定旋钮功能。

## 编译

```
qmk compile -kb czmao/08gamerst_uf2 -km uf2
qmk compile -kb czmao/08gamerst_uf2 -km via
```

仓库根目录的 `_build_uf2.sh` 是给本地 QMK_MSYS 环境跑 uf2 target 的
快捷脚本。

Keyboard Maintainer: [MAOKB](https://github.com/micahyy)
Hardware Supported: STM32F103CBT6 / APM32F103CBT6

![08gamerst](image.png)
