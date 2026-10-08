#define DYNAMIC_KEYMAP_LAYER_COUNT 8 //定义VIA层数

#ifdef ENCODER_ENABLE 
# define ENCODERS_PAD_A     { D2 }
#define  ENCODERS_PAD_B     { D3 }
#define ENCODER_RESOLUTIONS { 4 }/** 旋钮步进，QMK BUG 不要设置 ENCODER_RESOLUTION */
#define ENCODERS_MATRIX_MAP {{{4, 5}, {5, 5}}}
// #   define ENCODERS_TYPE_EC11
#endif
