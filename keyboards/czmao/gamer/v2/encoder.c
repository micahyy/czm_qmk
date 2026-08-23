#include <stdint.h>
#include <stdbool.h>
#include "quantum.h"

#ifdef VIA_ENABLE
#    include "dynamic_keymap.h"
#endif

// Encoder maps to matrix positions: clockwise -> [5,4], counter-clockwise -> [5,5]
// These positions in VIA can be remapped.
bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) {
        return false;
    }

    uint8_t row = clockwise ? 5 : 5;
    uint8_t col = clockwise ? 4 : 5;

    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    uint16_t keycode = KC_NO;

#ifdef VIA_ENABLE
    keycode = dynamic_keymap_get_keycode(layer, row, col);
    // If transparent, fall through layers
    while (keycode == KC_TRNS && layer > 0) {
        layer--;
        keycode = dynamic_keymap_get_keycode(layer, row, col);
    }
#else
    keycode = keymap_key_to_keycode(layer, (keypos_t){row, col});
#endif

    if (keycode != KC_NO && keycode != KC_TRNS) {
        tap_code_delay(keycode, 10);
    }

    return false;
}
