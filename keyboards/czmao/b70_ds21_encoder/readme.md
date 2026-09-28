# CZMAO b70_ds21_encoder

DS21 数字小键盘 + **4 个旋钮**，STM32F103CBT6 (Blue Pill)。

4 列 x 8 行矩阵。WS2812 RGB 接在 B13。

四个旋钮的引脚：

| 旋钮 | pin_a | pin_b | 顺时针位 | 逆时针位 |
| ---- | ----- | ----- | -------- | -------- |
| 0    | A1    | A2    | row 6 col 0 | row 6 col 1 |
| 1    | A3    | A4    | row 6 col 2 | row 6 col 3 |
| 2    | A5    | A6    | row 7 col 0 | row 7 col 1 |
| 3    | A7    | B0    | row 7 col 2 | row 7 col 3 |

> `keypos_t` 的成员顺序是 **{col, row}**，`ENCODERS_MATRIX_MAP` 里每一位都按这个顺序书写。
> row 6 / row 7 没有实体轴的物理位置，是留给旋钮的虚拟位，所以在每一层（含 VIA）都能单独改旋钮功能。

旋钮方向整体翻转（`ENCODER_DIRECTION_FLIP`），每个旋钮步进 4（`ENCODER_RESOLUTIONS`）。

Keyboard Maintainer: [MAOKB](https://github.com/micahyy)
Hardware Supported: STM32F103CBT6

Make example for this keyboard (after setting up your build environment):

    make czmao/b70_ds21_encoder:via

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.
