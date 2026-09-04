还没有完成，不要用


# CZMAO AT87

An 87-key (TKL) mechanical keyboard PCB powered by AT32F405RCT7-7, featuring 8000Hz USB polling.

* Keyboard Maintainer: [micahyy](https://github.com/micahyy)
* Hardware Supported: AT32F405RCT7-7 (LQFP64, 256KB Flash, 96KB SRAM, 216MHz, OTG-HS)
* Hardware Availability: CZMAO

## Bootloader

This firmware uses the [CherryUF2](https://github.com/zhaqian12/Cherryuf2) UF2 bootloader for AT32F405 (HS variant).

- Enter bootloader: double-tap the RESET button, or hold Esc while plugging in (Bootmagic)
- Flash firmware: drag and drop the `.uf2` file onto the `CherryUF2` USB drive
- App start address: `0x08004000` (16KB bootloader region)

For production programming, use the merged `*_production.bin` file with AT-Link (writes both bootloader and application at once).

## Building

```bash
make czmao/czmao_at87:default
make czmao/czmao_at87:via
```

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.
