#pragma once

#define DYNAMIC_KEYMAP_LAYER_COUNT 8

#ifdef ENCODER_ENABLE
#    define ENCODER_RESOLUTIONS   { 2 }
#    define ENCODERS_MATRIX_MAP   {{{4, 5}, {5, 5}}}
#endif
