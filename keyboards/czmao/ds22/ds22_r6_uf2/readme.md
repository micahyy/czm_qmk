# CZMAO DS22 R6 UF2

A 22-key hotswap numpad with per-key WS2812 RGB, powered by STM32F103.

* Keyboard Maintainer: [micahyy](https://github.com/micahyy)
* Hardware Supported: CZMAO DS22 R6 PCB (STM32F103, direct matrix, UF2 bootloader)
* Hardware Availability: [CZMAO on Taobao](https://caizimao.taobao.com)

## Bootloader

UF2 drag-and-drop flashing: double-tap RESET, a USB mass storage drive appears, drop the `.uf2` file onto it.

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (top-left key) and plug in the keyboard
* **Physical reset button**: Double-tap the RESET button on the PCB
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if available

First-time bootloader installation: flash the 16K UF2 bootloader (e.g. `uf2boot_czm_apm32f103.hex` in `bootloaders/`) at `0x08000000` with ST-Link.

## Building

Make sure you have the QMK firmware checked out and set up:

    qmk compile -kb czmao/ds22_r6_uf2 -km default
    qmk compile -kb czmao/ds22_r6_uf2 -km via

Flashing example for this keyboard:

    qmk flash -kb czmao/ds22_r6_uf2 -km default

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the
[make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.

## VIA

The `via` keymap has VIA enabled. Load the VIA JSON definition from the
[VIA definitions repo](https://github.com/micahyy/micahyy.github.io/tree/main/definitions/v3).

## Hardware Specs

| Spec | Details |
|------|---------|
| MCU | APM32F103CBT6 (STM32F103-compatible, 128KB flash) |
| Matrix | Direct (5 rows, non-rectangular, 22 keys) |
| Switches | Hotswap, 3-pin & 5-pin MX compatible |
| RGB | WS2812 per-key, QMK RGB Matrix (40+ effects) |
| Connector | USB-C |
| Bootloader | UF2 (16K boot @ 0x08000000, app @ 0x08004000) |
| PCB Color | White |
