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

## USB 识别

| | App | Bootloader |
|---|---|---|
| VID | `0x4E08` | `0x4E08` |
| PID | `0x4E0A` | `0x4E0B` |

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
| FN+ALT+Tab | 全键无冲 开/关 | 默认开 |
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
