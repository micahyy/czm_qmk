# CZMAO Gamer Lite

35-key one-handed gaming keyboard with RGB matrix (throne compatible).  
35键单手游戏键盘，RGB灯效（王座适配版）。

- Keyboard Maintainer: MAOKB
- Hardware Supported: STM32F103 (APM32F103CBT6 compatible)
- Hardware Availability: CZMAO

Make example for this keyboard (after setting up your build environment):

```
make czmao/gamer/lite:via
```

Flashing example:

```
make czmao/gamer/lite:via:flash
```

## Bootloader

Hold the top-left key (ESC) while plugging in to enter bootloader mode.  
按住左上角ESC键插入USB进入刷机模式。

## Note / 注意

USB VID/PID is `0x4A17:0x0005` and the product name stays "HID Keyboard Device" for game throne compatibility. Do not change the product name.  
USB VID/PID 为 `0x4A17:0x0005`，产品名保持 "HID Keyboard Device" 以适配游戏王座，产品名请勿修改。

Out of the box the board enumerates as a plain boot-protocol keyboard: 6KRO only, no mouse / media / NKRO reports, so consoles, phone thrones and phone OTG hosts accept it. PC users can switch NKRO, mouse keys and media keys back on from the VIA "Keyboard" tab (stored in EEPROM).  
出厂默认为标准 boot protocol 键盘：仅 6KRO，不发鼠标 / 媒体 / NKRO 报文，主机、王座、手机 OTG 均可识别。电脑用户可在 VIA "Keyboard" 页里打开 NKRO、鼠标键、媒体键（状态存 EEPROM）。

VIA definition (load manually in VIA): `via/czmao_gamer_lite_v11.json`.  
VIA 定义文件（在 VIA 里手动加载）：`via/czmao_gamer_lite_v11.json`。

## 3D Printed Case / 3D打印外壳

<https://makerworld.com.cn/zh/@micahyy/upload>

