/* EC87 bottom-out calibration: automatic adaptive learning +
 * VIA-channel data for the calibration web page.
 * See ec_calib.c for details. */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define EC_CALIB_KEYS       96      /* 6 rows x 16 cols */
#define EC_ROWS             6
#define EC_COLS             16
#define EC_CALIB_MAGIC      0x4543  /* "EC" */

/* Load pressed levels from flash. Call once after baseline calibration. */
void ec_calib_init(void);

/* Hand the release-baseline array (96 x uint16_t, owned by matrix.c) to the
 * calibration module; thresholds are computed against live baseline. */
void ec_calib_set_baseline(const uint16_t *bl);

/* Per-key thresholds against the live baseline (ADC counts). Falls back to
 * legacy fixed deltas for keys without a learned bottom-out level. */
uint16_t ec_press_delta(uint8_t idx);
uint16_t ec_release_delta(uint8_t idx);

/* Feed one raw ADC sample for key idx (row*16+col), every matrix scan.
 * Drives adaptive learning + live depth state. */
void ec_calib_sample(uint8_t idx, uint16_t v);

/* --- VIA custom channel API (used by the driver-site calibration page) --- */

/* Copy one keyboard row (16 bytes) of live travel depth, 0-100 %. */
void ec_via_get_row(uint8_t row, uint8_t *out);

/* Copy the 12-byte learned-state bitmap (96 bits, bit = key learned). */
void ec_via_get_learned(uint8_t *out);

/* How many keys have a learned bottom-out level (0-96). */
uint8_t ec_via_learned_count(void);

/* Web-page commands. */
void ec_via_start_calib(void);   /* begin walk-through (resets idle timer) */
bool   ec_via_save(void);        /* flush learned levels to flash now */
void   ec_via_reset(void);       /* erase learned levels + flash page */

/* Calibration mute: while active, sensing/learning/depth keep running but
 * the matrix reported to the host is forced empty — no key output at all,
 * like a keyboard-test tool. Auto-disables after 10 min of no key activity
 * as a safety net (e.g. web page closed while muted). */
bool ec_mute_active(void);
void ec_mute_set(bool on);
void ec_mute_poll(void);         /* call every matrix scan */
