# CZMAO DS17R6

17-key right-hand number pad with per-key RGB, STM32F103 controller.

* Keyboard Maintainer: CZMAO
* Hardware Supported: CZMAO DS17R6 (STM32F103, direct matrix, WS2812 RGB)

## Building

    make CZMAO/ds17r6:default
    make CZMAO/ds17r6:via

## Bootloader

* **Bootmagic reset**: Hold down FN/Num Lock while plugging in
* **Physical reset**: Short the reset contacts on the back of the PCB

## Keymap Layers

* Layer 0: Standard numpad; FN = Tap Num Lock / Hold Layer 1
* Layer 1: RGB controls; MO(2) on key [0,2]
* Layer 2: NK_TOGG (minus key), EE_CLR factory reset (Enter key)
* Layer 7: EE_CLR factory reset (Enter key) — VIA hidden layer
