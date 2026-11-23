# CZMAO DS22 R6 AT32

A 22-key hotswap numpad with per-key WS2812 RGB, powered by AT32F403A
(pin-compatible drop-in for the STM32F103 variant).

> **Hardware verification pending — do not ship to end users yet.**
> AT32F403A's B13 only exposes TMR1_CH1N (complementary), which QMK's
> `ws2812_pwm.c` does not drive, so this board falls back to
> `WS2812_DRIVER = bitbang`. The sibling F103/APM32 boards had to leave
> bit-bang for exactly this reason — flash wait states stretch the 0-bit
> high past WS2812's ~0.5 us logic-1 threshold and the strip locks solid
> white. AT32F403A runs at 192 MHz with different flash timing than
> F103, so the same failure may or may not reproduce. Verify on real
> silicon before publishing a UF2; if it does reproduce, tune
> `WS2812_BITBANG_NOP_FUDGE` in `platforms/chibios/drivers/ws2812_bitbang.c`
> or run the driver from RAM. Do not switch the whole board over to
> PWM+DMA without first checking whether a `ws2812_pwm` AT32
> complementary-output path exists in your ChibiOS contrib tree.

* Keyboard Maintainer: [micahyy](https://github.com/micahyy)
* Hardware Supported: CZMAO DS22 R6 PCB (AT32F403ACCT7, direct matrix, UF2 bootloader)
* Hardware Availability: [CZMAO on Taobao](https://caizimao.taobao.com)

## Bootloader

CherryUF2 (AT32F403A port, HICK USB clock). UF2 drag-and-drop flashing: double-tap
RESET, a USB mass storage drive appears, drop the `.uf2` file onto it.

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (top-left key) and plug in the keyboard
* **Physical reset button**: Double-tap the RESET button on the PCB
* **Keycode in layout**: Press the key mapped to `QK_BOOT` (writes the CherryUF2
  double-tap magic via QMK's `tinyuf2.c` and resets)

First-time bootloader installation: flash the prebuilt bootloader
(`bootloaders/czm_at32f403_uf2boot.bin`, ~10KB) at `0x08000000` with ST-Link/J-Link.

## Building

    qmk compile -kb czmao/ds22_r6_at32 -km default
    qmk compile -kb czmao/ds22_r6_at32 -km via

The build outputs `ds22_r6_at32_default.bin`. Convert it to UF2 (QMK's uf2conv.py
does not know the AT32 family id):

    python scripts/bin2uf2.py --base-address 0x08004000 --family-id 0x98a285b9 \
        ds22_r6_at32_default.bin ds22_r6_at32_default.uf2

## VIA

The `via` keymap has VIA enabled. Load the VIA JSON definition from the
[VIA definitions repo](https://github.com/micahyy/micahyy.github.io/tree/main/definitions/v3).

## Hardware Specs

| Spec | Details |
|------|---------|
| MCU | AT32F403ACCT7 (STM32F103 pin-compatible, LQFP48, 256KB flash, 96KB SRAM) |
| Clock | HEXT 8MHz x24 = 192MHz, USB = PLL/4 = 48MHz (FS @ PA11/PA12) |
| Matrix | Direct (5 rows, non-rectangular, 22 keys) |
| Switches | Hotswap, 3-pin & 5-pin MX compatible |
| RGB | WS2812 per-key, QMK RGB Matrix (40+ effects) |
| Connector | USB-C |
| Bootloader | CherryUF2 (16K boot @ 0x08000000, app @ 0x08004000, 4KB WL EEPROM @ 0x0803F000) |
| PCB Color | White |
