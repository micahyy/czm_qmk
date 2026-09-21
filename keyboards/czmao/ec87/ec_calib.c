/* EC87 bottom-out calibration — automatic adaptive learning.
 *
 * Two values describe every key:
 *   baseline  - release (resting) ADC level, relearned at every boot by
 *               matrix.c with no hands on the keyboard
 *   pressed   - bottom-out ADC level, learned automatically in the
 *               background while typing, persisted to the last flash page
 *
 * Learning rule: whenever a key is held (a few scans of travel past the
 * actuation point) and then released, the average held level is compared
 * with the stored bottom-out level:
 *   - no stored value yet            -> learn it
 *   - new reading is clearly deeper  -> learn it
 *   - new reading is clearly shallower (>30 %) -> learn it (spring/age
 *     drift); small differences are ignored to resist noise
 * Flash is written at most once per idle pause (5 s after the last key
 * press) so a typing session costs a single page erase.
 *
 * Thresholds derive from the live travel span = pressed - baseline:
 * actuation at 40 %, release at 25 %, with fixed floors. Keys without a
 * learned level fall back to the legacy fixed deltas.
 *
 * Weak-key tolerance: a spring that is not seated perfectly (tilted, off
 * centre, partly lifted) couples far less capacitance, so its whole travel
 * may be only 30-100 ADC counts instead of 200+. The entry gate and the
 * accepted minimum span are therefore set just above the ADC noise floor,
 * and the deeper-reading update margin scales with the span instead of a
 * fixed value. Such keys learn and calibrate like any other; hysteresis
 * (press floor > release floor) plus debounce keeps them from chattering.
 *
 * A VIA custom channel (see keymaps/via/keymap.c) exposes live per-key
 * travel depth and learned-state bitmap for the calibration web page,
 * plus start/save/reset commands.
 */
#include "ec_calib.h"
#include "ec_flash.h"
#include "quantum.h"
#include "timer.h"
#include <string.h>

/* Tuning */
#define LEGACY_PRESS_DELTA   100
#define LEGACY_RELEASE_DELTA 60
#define HOLD_ENTRY_DELTA     20    /* raw delta that counts as physically held;
                                      kept just above the ADC noise floor so a
                                      mis-seated weak spring still accumulates */
#define LEARN_MIN_SAMPLES    6     /* scans held before a release is accepted */
#define LEARN_MIN_SPAN       30    /* smallest accepted travel: the full
                                      travel of a weak spring may be only
                                      30-100 counts; still far above noise */
#define UPDATE_MARGIN_MIN    20    /* deeper reading beats stored by at least
                                      this or UPDATE_MARGIN_FRACT of span */
#define UPDATE_MARGIN_FRACT  8     /* whichever is larger, as % of span */
#define SHALLOW_FRACT        70    /* shallower reading below 70 % re-learns */
#define PRESS_FRACT          40    /* actuation at 40 % of learned travel */
#define RELEASE_FRACT        25    /* release at 25 % of learned travel */
#define FLOOR_PRESS          18    /* weakest usable actuation threshold;
                                      above noise, hysteresis vs release */
#define FLOOR_RELEASE        10
#define SAVE_IDLE_MS         5000  /* flush to flash after this much quiet */

#define INVALID 0xFFFFu

struct ec_record {
    uint16_t magic;
    uint16_t version;
    uint16_t pressed[EC_CALIB_KEYS];
    uint16_t crc;
};

/* RAM state ---------------------------------------------------------- */
static uint16_t pressed_lvl[EC_CALIB_KEYS];
static const uint16_t *baseline;

/* per-key press accumulator (current press) */
static uint32_t sum[EC_CALIB_KEYS];
static uint16_t cnt[EC_CALIB_KEYS];
static uint8_t  held[EC_CALIB_KEYS];

/* live UI state */
static uint8_t  depth[EC_CALIB_KEYS];      /* 0-100 % of travel */
static uint8_t  learned_map[(EC_CALIB_KEYS + 7) / 8];

/* flash flush scheduling */
static bool     dirty;
static uint32_t last_activity;

/* calibration mute: sensing keeps running, host matrix forced empty */
static bool     muted;
#define MUTE_TIMEOUT_MS 600000   /* 10 min safety auto-release */

bool ec_mute_active(void) { return muted; }

void ec_mute_set(bool on) {
    muted = on;
    last_activity = timer_read32();
}

void ec_mute_poll(void) {
    if (muted && timer_elapsed(last_activity) > MUTE_TIMEOUT_MS)
        muted = false;
}

/* CRC-16/CCITT ------------------------------------------------------- */
static uint16_t crc16(const uint16_t *p, uint32_t halfwords) {
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < halfwords; i++) {
        crc ^= p[i];
        for (int b = 0; b < 16; b++)
            crc = (crc & 1) ? (crc >> 1) ^ 0x8408 : crc >> 1;
    }
    return crc;
}

static void mark_learned(uint8_t idx, bool on) {
    if (on) learned_map[idx >> 3] |=  (uint8_t)(1u << (idx & 7));
    else    learned_map[idx >> 3] &= (uint8_t)~(1u << (idx & 7));
}

/* Persistence -------------------------------------------------------- */
static bool store_flush(void) {
    static uint16_t buf[EC_STORE_PAGE_SIZE / 2];
    memset(buf, 0xFF, sizeof(buf));
    struct ec_record *rec = (struct ec_record *)buf;
    rec->magic = EC_CALIB_MAGIC;
    rec->version = 1;
    memcpy(rec->pressed, pressed_lvl, sizeof(pressed_lvl));
    rec->crc = crc16(buf, offsetof(struct ec_record, crc) / 2);

    bool ok = ec_store_erase() &&
              ec_store_write_halfwords(buf, EC_STORE_PAGE_SIZE / 2);
    dirty = false;
    return ok;
}

void ec_calib_init(void) {
    for (int i = 0; i < EC_CALIB_KEYS; i++) pressed_lvl[i] = INVALID;

    const struct ec_record *rec = (const struct ec_record *)ec_store_ptr();
    if (rec->magic == EC_CALIB_MAGIC && rec->version == 1) {
        uint16_t calc = crc16((const uint16_t *)rec,
                              offsetof(struct ec_record, crc) / 2);
        if (calc == rec->crc) {
            memcpy(pressed_lvl, (const void *)rec->pressed,
                   sizeof(pressed_lvl));
        }
    }
    memset(learned_map, 0, sizeof(learned_map));
    for (int i = 0; i < EC_CALIB_KEYS; i++)
        if (pressed_lvl[i] != INVALID) mark_learned(i, true);
}

void ec_calib_set_baseline(const uint16_t *bl) { baseline = bl; }

uint16_t ec_press_delta(uint8_t idx) {
    if (idx >= EC_CALIB_KEYS || pressed_lvl[idx] == INVALID || !baseline)
        return LEGACY_PRESS_DELTA;
    int32_t span = (int32_t)pressed_lvl[idx] - (int32_t)baseline[idx];
    if (span < LEARN_MIN_SPAN) return LEGACY_PRESS_DELTA;
    uint32_t d = (uint32_t)span * PRESS_FRACT / 100;
    return (d < FLOOR_PRESS) ? FLOOR_PRESS : (uint16_t)d;
}

uint16_t ec_release_delta(uint8_t idx) {
    if (idx >= EC_CALIB_KEYS || pressed_lvl[idx] == INVALID || !baseline)
        return LEGACY_RELEASE_DELTA;
    int32_t span = (int32_t)pressed_lvl[idx] - (int32_t)baseline[idx];
    if (span < LEARN_MIN_SPAN) return LEGACY_RELEASE_DELTA;
    uint32_t d = (uint32_t)span * RELEASE_FRACT / 100;
    return (d < FLOOR_RELEASE) ? FLOOR_RELEASE : (uint16_t)d;
}

/* Per-scan feed ------------------------------------------------------ */
void ec_calib_sample(uint8_t idx, uint16_t v) {
    if (idx >= EC_CALIB_KEYS || !baseline) return;

    int32_t bl = baseline[idx];
    int32_t d  = (int32_t)v - bl;

    /* live depth for the UI */
    int32_t span = (pressed_lvl[idx] != INVALID)
                       ? (int32_t)pressed_lvl[idx] - bl
                       : (int32_t)HOLD_ENTRY_DELTA * 4;
    if (span < LEARN_MIN_SPAN) span = LEARN_MIN_SPAN;
    int32_t pct = d * 100 / span;
    depth[idx] = (pct <= 0) ? 0 : (pct > 100 ? 100 : (uint8_t)pct);

    /* press accumulation */
    if (d > HOLD_ENTRY_DELTA) {
        sum[idx] += v;
        cnt[idx]++;
        held[idx] = 1;
        last_activity = timer_read32();
    } else if (held[idx]) {
        if (cnt[idx] >= LEARN_MIN_SAMPLES) {
            uint16_t avg = (uint16_t)(sum[idx] / cnt[idx]);
            int32_t travel = (int32_t)avg - bl;
            if (travel >= LEARN_MIN_SPAN) {
                bool learn = false;
                /* adaptive deeper-margin: scale with span but keep a small
                 * fixed floor so weak keys can still update */
                int32_t margin = travel * UPDATE_MARGIN_FRACT / 100;
                if (margin < UPDATE_MARGIN_MIN) margin = UPDATE_MARGIN_MIN;
                if (pressed_lvl[idx] == INVALID) {
                    learn = true;
                } else if ((int32_t)avg > (int32_t)pressed_lvl[idx] + margin) {
                    learn = true;                 /* clearly deeper */
                } else if ((int32_t)avg < (int32_t)pressed_lvl[idx] * SHALLOW_FRACT / 100) {
                    learn = true;                 /* clearly shallower (drift) */
                }
                if (learn) {
                    pressed_lvl[idx] = avg;
                    mark_learned(idx, true);
                    dirty = true;
                }
            }
        }
        sum[idx] = 0;
        cnt[idx] = 0;
        held[idx] = 0;
    }

    /* flush to flash once typing goes quiet */
    if (dirty && timer_elapsed(last_activity) > SAVE_IDLE_MS) {
        store_flush();
    }
}

/* VIA custom channel hooks ------------------------------------------ */
void ec_via_get_row(uint8_t row, uint8_t *out) {
    if (row >= EC_ROWS) { memset(out, 0, EC_COLS); return; }
    memcpy(out, &depth[row * EC_COLS], EC_COLS);
}

void ec_via_get_learned(uint8_t *out) {
    memcpy(out, learned_map, sizeof(learned_map));
}

uint8_t ec_via_learned_count(void) {
    uint8_t n = 0;
    for (int i = 0; i < EC_CALIB_KEYS; i++)
        if (pressed_lvl[i] != INVALID) n++;
    return n;
}

void ec_via_start_calib(void) {
    /* The web page walks every key; learning is already live, this just
     * makes sure a flush of whatever gets learned happens promptly. */
    last_activity = timer_read32();
}

bool ec_via_save(void) {
    return store_flush();
}

void ec_via_reset(void) {
    for (int i = 0; i < EC_CALIB_KEYS; i++) pressed_lvl[i] = INVALID;
    memset(learned_map, 0, sizeof(learned_map));
    /* erase the page so defaults survive power cycle */
    dirty = false;
    (void)ec_store_erase();
}
