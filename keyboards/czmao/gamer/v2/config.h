#pragma once

#define DYNAMIC_KEYMAP_LAYER_COUNT 8

#ifdef ENCODER_ENABLE
#    define ENCODERS_PAD_A        { A5 }
#    define ENCODERS_PAD_B        { A6 }
#    define ENCODER_RESOLUTIONS   { 2 }
#    define ENCODERS_MATRIX_MAP   {{{4, 5}, {5, 5}}}
#endif
