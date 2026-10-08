# CZMAO Gamer

One-handed gaming keyboard series.
单手游戏键盘系列。

| Version | MCU | Keys | Encoder | USB VID:PID | Description |
|---------|-----|------|---------|-------------|-------------|
| [gamer_en](gamer_en/) | STM32F103 | 37 | Yes | 0x4E08:0x4E08 | Diode matrix, rotary encoder, RGB matrix |
| [at](at/) | ATmega32U4 | 37 | Yes | 0x4E08:0x4E08 | Legacy AT version |
| [atr2](atr2/) | ATmega32U4 | 37 | Yes | 0x4E08:0x4E08 | Legacy AT R2 |
| [atr2_light](atr2_light/) | ATmega32U4 | 37 | Yes | 0x4E08:0x4E08 | Legacy AT R2 Light |
| [lite](lite/) | AT32F403A / STM32F103 | 35 | No | 0x4A17:0x0005 | Direct pin matrix, RGB matrix, throne compatible |

## Compilation / 编译

    make czmao/gamer/gamer_en:via
    make czmao/gamer/at:via
    make czmao/gamer/atr2:via
    make czmao/gamer/atr2_light:via
    make czmao/gamer/lite:via

## 3D Printed Case / 3D打印外壳

https://makerworld.com.cn/zh/@micahyy/upload
