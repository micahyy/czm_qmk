# CZMAO left64 RGB

64-key compact (65%-class) keyboard with per-key WS2812 RGB matrix, STM32F103 controller.

* Keyboard Maintainer: [micah](https://github.com/micahyy)
* Hardware Supported: CZMAO left64 RGB (STM32F103, COL2ROW matrix, WS2812 RGB)
* Hardware Availability: CZMAO / 菜籽猫客制化键盘

## Building

Make example for this keyboard (after setting up your build environment):

    make czmao/left64:default
    make czmao/left64:via

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

* **Bootmagic reset**: Hold down the top-left key (Esc) while plugging in
* **Physical reset**: Short the reset contacts on the back of the PCB

## Keymap Layers

* Layer 0: Standard QWERTY
* Layer 1: F-keys, media/volume, navigation cluster, RGB controls
* Layer 2: RGB/backlight adjustment, EE_CLR factory reset, NKRO toggle, QK_BOOT bootloader (via keymap only)
