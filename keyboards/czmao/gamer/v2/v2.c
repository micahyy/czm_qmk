#include "quantum.h"
#include "wait.h"

void keyboard_pre_init_kb(void) {
    // WS2812 reset: hold data line low
    palSetLineMode(B13, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(B13);
    wait_us(300);

    keyboard_pre_init_user();
}

void matrix_init_kb(void) {
    rgb_matrix_set_color_all(0, 0, 0);
    matrix_init_user();
}
