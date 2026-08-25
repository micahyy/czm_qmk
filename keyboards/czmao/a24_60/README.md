# CZMAO A24 60% Multi-layout

A 60% multi-layout mechanical keyboard (supports ISO, split backspace, split shift), based on STM32F103.

* Keyboard Maintainer: [micah](https://github.com/micahyy)
* Hardware Supported: CZMAO A24 PCB (STM32F103, stm32duino bootloader)
* Hardware Availability: CZMAO

Make example for this keyboard (after setting up your build environment):

    make czmao/a24_60:default

Flashing example for this keyboard:

    make czmao/a24_60:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with the [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 2 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the PCB
