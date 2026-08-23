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
make czmao/ec87:default
```

## Flashing

Enter STM32duino bootloader mode (hold BOOT0 + press RESET), then:

```bash
make czmao/ec87:default:flash
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
