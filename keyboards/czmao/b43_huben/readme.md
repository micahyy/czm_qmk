# CZMAO b43_huben

USB_EN 桌面小控制器，STM32F103CBT6 (Blue Pill)，3 列 x 3 行矩阵，4 颗 WS2812 RGB 接在 B13。

7 个位置中有 2 个（row 2 的 ENL / ENR）没有实体轴，是留给旋钮的虚拟位。

旋钮：C13 / C14，步进 4（`ENCODER_RESOLUTION`）。`ENCODERS_MATRIX_MAP` 把顺时针映射到 ENR、逆时针映射到 ENL，因此可以在每一层（含 VIA）单独指定旋钮功能。

Keyboard Maintainer: [MAOKB](https://github.com/micahyy)
Hardware Supported: STM32F103CBT6

Make example for this keyboard (after setting up your build environment):

    make czmao/b43_huben:via

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.
