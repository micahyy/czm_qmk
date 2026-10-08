#define DYNAMIC_KEYMAP_LAYER_COUNT 4 //定义VIA层数

#ifdef ENCODER_ENABLE 
# define ENCODERS_PAD_A     { B11,              B9,              B7,                B12 }
#define  ENCODERS_PAD_B     { B10 ,             C15,             B8,                B14 }
#define ENCODER_RESOLUTIONS { 4,                4,               4,                 4   }/** 旋钮步进，QMK BUG 不要设置 ENCODER_RESOLUTION */
#define ENCODERS_MATRIX_MAP {{{0, 0}, {0, 1}}, {{0, 4}, {1, 4}}, {{0, 3}, {1, 3}}, {{0, 2}, {1, 2}}}
// #   define ENCODERS_TYPE_EC11
#endif
