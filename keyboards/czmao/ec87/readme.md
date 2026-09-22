# CZMAO EC87

87-key TKL electrostatic capacitive keyboard with APM32F103CBT6 MCU and 74HC4067 analog mux.

* Keyboard Maintainer: [micahyy](https://github.com/micahyy)
* Hardware Supported: CZMAO EC87 PCB (APM32F103CBT6)
* Hardware Availability: CZMAO

## Hardware Specs

| Item | Detail |
|------|--------|
| MCU | APM32F103CBT6 (STM32F103 compatible) |
| Switch | Electrostatic Capacitive |
| Key Count | 87 (TKL ANSI) |
| Matrix | 6 rows × 16 cols (74HC4067 mux + ADC) |
| RGB | WS2812 × 26 |
| USB | Full Speed (PA11/PA12) |

## Pin Map

| Pin | Function |
|-----|----------|
| PA1 | ADC input (74HC4067 COM) |
| PA2-PA7 | Row drive 0-5 |
| PB8 | 74HC4067 S0 |
| PB9 | 74HC4067 S1 |
| PB10 | 74HC4067 S2 |
| PB11 | 74HC4067 S3 |
| PB12 | 74HC4067 EN (active low) |
| PA8 | WS2812 RGB data |
| PA11 | USB D- |
| PA12 | USB D+ |

## Building

```bash
make czmao/ec87:via
```

## 进入 Bootloader / 刷固件（快捷键）

无需拆机或按 BOOT0，键盘底部一排同时按住以下三个键即可进入 bootloader（固件烧录模式）：

**Fn ＋ 右 Alt（RAlt）＋ 左 Ctrl**

- 按住 **Fn** 和 **右 Alt** 会切换到 FN2 层，该层的 **左 Ctrl** 位置就是 `QK_BOOT`，三键同按即进入 bootloader。
- 三键同按住约 1 秒，键盘会进入 UF2/STM32duino bootloader，此时电脑会出现一个 U 盘或烧录端口，即可拖入固件或执行烧录。
- 在 VIA 中切到第 3 层（FN2 层），左下角的格子会直接显示 `QK_BOOT` 键值，可自行查看或修改。
- 注意：这不是普通重启，而是进入烧录模式；正常使用不会误触。

## Flashing

Enter STM32duino bootloader mode with the shortcut **Left Ctrl + Fn + Right Alt** above
(or hold BOOT0 + press RESET), then:

```bash
make czmao/ec87:via:flash
```

## Capacitive Sensing

The keyboard uses a custom matrix driver (`matrix.c`) that measures capacitance via the internal ADC. Each key position is scanned by:

1. Discharging the electrode (drive row low)
2. Charging through the key capacitor (drive row high)
3. Reading voltage via 74HC4067 + ADC
4. Comparing against a calibrated baseline

Key parameters in `matrix.c` may need tuning for different switch variants:
- `THRESHOLD`: ADC difference to trigger press (default 150)
- `CHARGE_US`: charging time in microseconds (default 10)
- `DISCHARGE_US`: discharge time in microseconds (default 5)

Baseline is calibrated on boot — do not press keys during startup.

## VIA

VIA is supported. Load the definition file `keymaps/via/1243021316.json` in VIA's "Load Draft Definition".
