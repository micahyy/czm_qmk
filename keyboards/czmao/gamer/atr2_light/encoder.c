
#include <stdint.h>
#include <stdbool.h>
#include "matrix.h"
#include "quantum.h"

#ifdef ENCODERS_MATRIX_MAP
static keypos_t encoders_matrix_map[NUM_ENCODERS_MAX_PER_SIDE][2] = ENCODERS_MATRIX_MAP;
#endif

bool encoder_update_kb(uint8_t index, bool clockwise)
{
#ifndef ENCODERS_MATRIX_MAP
    return encoder_update_user(index, clockwise);
#else
    keypos_t key = encoders_matrix_map[index][!clockwise];
    uint8_t layerIndex = layer_switch_get_layer(key);
    uint16_t keycode = keymap_key_to_keycode(layerIndex, key);

    if (keycode != KC_NO)
    {
        tap_code_delay(keycode, 0);
        return false;
    }
    else
    {
        return true;
    }
#endif
}


