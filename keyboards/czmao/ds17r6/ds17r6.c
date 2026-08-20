#include "quantum.h"
#include "gpio.h"
#include "wait.h"

// WS2812 data line reset: hold B13 low for >280us to reset LED strip,
// preventing random green LED on plug-in.
void keyboard_pre_init_kb(void) {
    setPinOutput(B13);
    writePinLow(B13);
    wait_us(300);
    keyboard_pre_init_user();
}

void matrix_init_kb(void) {
    rgb_matrix_set_color_all(0, 0, 0);
    matrix_init_user();
}
