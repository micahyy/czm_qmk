# CZMAO b23_ds34_v2

DS34 v2 数字小键盘 + 1 旋钮，STM32F103CBT6 (Blue Pill)。

8 列 x 6 行矩阵，带方向键、编辑区与音量键，WS2812 RGB 接在 B13。

旋钮：B8 / B9，方向翻转（`ENCODER_DIRECTION_FLIP`）。`ENCODERS_MATRIX_MAP` 映射到 row 3 的 col 0 / col 1 两个虚拟位，因此可以在每一层（含 VIA）单独指定旋钮功能。

Keyboard Maintainer: [MAOKB](https://github.com/micahyy)
Hardware Supported: STM32F103CBT6

Make example for this keyboard (after setting up your build environment):

    make czmao/b23_ds34_v2:via

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.
