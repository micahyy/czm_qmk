/* EC87 v4.3 — capacitive matrix with flash-persistent auto-calibration
 *
 * Features:
 * - First boot: auto-calibrate all 96 keys, save baseline to internal flash
 * - Normal boot: load baseline from flash, ready instantly
 * - Hold any key during USB plug-in: force recalibration
 * - Runtime slow drift compensation (EMA, ~0.4%/scan)
 * - Hysteresis + 5ms debounce
 * - Non-scanned rows driven LOW as active shield
 *
 * Flash layout: app uses 0x08004000-0x0800B7FF (46KB)
 *               calib page: 0x0800B800-0x0800BBFF (1KB, page 46)
 */
#include "matrix.h"
#include "gpio.h"
#include "wait.h"
#include "print.h"
#include "debug.h"
#include <stdint.h>
#include <string.h>

#define ROW_COUNT 6
#define COL_COUNT 16
#define TOTAL_KEYS (ROW_COUNT * COL_COUNT)

static const pin_t mux_pins[4] = {B8, B9, B10, B11};
#define MUX_EN_PIN B12

/* ── ADC registers ── */
#define EC_RCC_APB2ENR  (*(volatile uint32_t *)0x40021018)
#define EC_RCC_CFGR     (*(volatile uint32_t *)0x40021004)
#define EC_ADC1_SR      (*(volatile uint32_t *)0x40012400)
#define EC_ADC1_CR1     (*(volatile uint32_t *)0x40012404)
#define EC_ADC1_CR2     (*(volatile uint32_t *)0x40012408)
#define EC_ADC1_SMPR1   (*(volatile uint32_t *)0x4001240C)
#define EC_ADC1_SMPR2   (*(volatile uint32_t *)0x40012410)
#define EC_ADC1_SQR1    (*(volatile uint32_t *)0x4001242C)
#define EC_ADC1_SQR3    (*(volatile uint32_t *)0x40012434)
#define EC_ADC1_DR      (*(volatile uint32_t *)0x4001244C)
#define EC_GPIOA_CRL    (*(volatile uint32_t *)0x40010800)
#define EC_GPIOA_CRH    (*(volatile uint32_t *)0x40010804)
#define EC_GPIOA_BSRR   (*(volatile uint32_t *)0x40010810)

/* ── Flash registers ── */
#define EC_FLASH_ACR    (*(volatile uint32_t *)0x40022000)
#define EC_FLASH_KEYR   (*(volatile uint32_t *)0x40022004)
#define EC_FLASH_SR     (*(volatile uint32_t *)0x4002200C)
#define EC_FLASH_CR     (*(volatile uint32_t *)0x40022010)
#define EC_FLASH_AR     (*(volatile uint32_t *)0x40022014)
#define CALIB_FLASH_ADDR 0x0800B800u
#define CALIB_MAGIC 0xEC87CA1Bu

/* ── Tuning ── */
#define DISCHARGE_US    10
#define SETTLE_US       5
#define MUX_SETTLE_US   3
#define CALIB_SAMPLES   32
#define PRESS_DELTA     120
#define RELEASE_DELTA    80
#define DEBOUNCE_MS      5
#define DRIFT_ALPHA      4   /* EMA: baseline += (raw - baseline) / 256 * DRIFT_ALPHA */
#define BOOT_KEY_THRESH  300

/* ── Calibration block stored in flash ── */
typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint16_t baseline[TOTAL_KEYS];
    uint16_t checksum;
} calib_block_t;

static uint16_t baseline[TOTAL_KEYS];
static uint8_t  key_state[TOTAL_KEYS];
static uint32_t last_change[TOTAL_KEYS];

/* ── Low-level helpers ── */
static void mux_set(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

static void adc_init(void) {
    EC_RCC_APB2ENR |= (1u<<2)|(1u<<3)|(1u<<9);
    EC_RCC_CFGR = (EC_RCC_CFGR & ~(3u<<14)) | (2u<<14);
    EC_ADC1_CR2 = (1u<<0);
    wait_ms(2);
    EC_ADC1_CR2 |= (1u<<3);
    { volatile uint32_t t=200000; while((EC_ADC1_CR2&(1u<<3))&&--t); }
    EC_ADC1_CR2 |= (1u<<2);
    { volatile uint32_t t=200000; while((EC_ADC1_CR2&(1u<<2))&&--t); }
    EC_ADC1_CR1 = 0;
    EC_ADC1_CR2 = (1u<<0)|(1u<<20)|(7u<<17);
    EC_ADC1_SMPR2 = 0x2DB6DB6D;
    EC_ADC1_SMPR1 = 0x00FFFFFF;
    EC_ADC1_SQR1 = 0;
}

static inline uint16_t adc_read(uint8_t ch) {
    EC_ADC1_SQR3 = ch & 0x1F;
    EC_ADC1_CR2 |= (1u<<22);
    while(!(EC_ADC1_SR&(1u<<1)));
    return (uint16_t)(EC_ADC1_DR & 0xFFFu);
}

static inline void set_pin_output(uint8_t pin, uint8_t high) {
    uint8_t sh = pin * 4;
    volatile uint32_t *cr = (pin < 8) ? &EC_GPIOA_CRL : &EC_GPIOA_CRH;
    if (pin >= 8) sh -= 32;
    *cr = (*cr & ~(0xFu << sh)) | (0x3u << sh);
    if (high) EC_GPIOA_BSRR = (1u << pin);
    else      EC_GPIOA_BSRR = (1u << (pin+16));
}

static inline void set_pin_analog(uint8_t pin) {
    uint8_t sh = pin * 4;
    volatile uint32_t *cr = (pin < 8) ? &EC_GPIOA_CRL : &EC_GPIOA_CRH;
    if (pin >= 8) sh -= 32;
    *cr &= ~(0xFu << sh);
}

/* ── Key read (charge redistribution) ── */
static uint16_t read_key(uint8_t row, uint8_t col) {
    uint8_t pin = row + 2;
    set_pin_output(1, 0);
    for (uint8_t r=0;r<ROW_COUNT;r++) set_pin_output(r+2, 0);
    wait_us(DISCHARGE_US);
    mux_set(col);
    wait_us(MUX_SETTLE_US);
    set_pin_analog(pin);
    set_pin_output(1, 1);
    wait_us(SETTLE_US);
    uint16_t v = adc_read(pin);
    set_pin_output(1, 0);
    set_pin_output(pin, 0);
    return v;
}

/* ── Flash operations ── */
static void flash_unlock(void) {
    EC_FLASH_KEYR = 0x45670123;
    EC_FLASH_KEYR = 0xCDEF89AB;
}

static void flash_lock(void) {
    EC_FLASH_CR |= (1u<<7);
}

static void flash_erase_page(uint32_t addr) {
    while(EC_FLASH_SR & (1u<<0));
    EC_FLASH_CR |= (1u<<1);   /* PER */
    EC_FLASH_AR = addr;
    EC_FLASH_CR |= (1u<<6);   /* STRT */
    while(EC_FLASH_SR & (1u<<0));
    EC_FLASH_CR &= ~(1u<<1);
}

static void flash_write_halfword(uint32_t addr, uint16_t data) {
    while(EC_FLASH_SR & (1u<<0));
    EC_FLASH_CR |= (1u<<0);   /* PG */
    *(volatile uint16_t *)addr = data;
    while(EC_FLASH_SR & (1u<<0));
    EC_FLASH_CR &= ~(1u<<0);
}

static uint16_t calc_checksum(const uint16_t *data, int count) {
    uint32_t sum = 0;
    for (int i=0;i<count;i++) sum += data[i];
    return (uint16_t)(sum & 0xFFFF);
}

static bool calib_load(void) {
    const calib_block_t *blk = (const calib_block_t *)CALIB_FLASH_ADDR;
    if (blk->magic != CALIB_MAGIC) return false;
    if (blk->checksum != calc_checksum(blk->baseline, TOTAL_KEYS)) return false;
    memcpy(baseline, blk->baseline, sizeof(baseline));
    return true;
}

static void calib_save(void) {
    calib_block_t blk;
    blk.magic = CALIB_MAGIC;
    memcpy(blk.baseline, baseline, sizeof(baseline));
    blk.checksum = calc_checksum(baseline, TOTAL_KEYS);

    flash_unlock();
    flash_erase_page(CALIB_FLASH_ADDR);

    const uint16_t *p = (const uint16_t *)&blk;
    uint32_t addr = CALIB_FLASH_ADDR;
    for (unsigned i=0; i<(sizeof(blk)+1)/2; i++) {
        flash_write_halfword(addr, p[i]);
        addr += 2;
    }
    flash_lock();
}

/* ── Calibration ── */
static bool any_key_held(void) {
    for (uint8_t c=0;c<COL_COUNT;c++) {
        for (uint8_t r=0;r<ROW_COUNT;r++) {
            if (read_key(r, c) > BOOT_KEY_THRESH) return true;
        }
    }
    return false;
}

static void run_calibration(void) {
    /* Warm-up */
    for (uint8_t c=0;c<COL_COUNT;c++)
        for (uint8_t r=0;r<ROW_COUNT;r++)
            read_key(r, c);

    uint32_t acc[TOTAL_KEYS];
    memset(acc, 0, sizeof(acc));

    for (int s=0;s<CALIB_SAMPLES;s++) {
        for (uint8_t c=0;c<COL_COUNT;c++) {
            for (uint8_t r=0;r<ROW_COUNT;r++) {
                acc[r*COL_COUNT+c] += read_key(r, c);
            }
        }
    }
    for (int i=0;i<TOTAL_KEYS;i++)
        baseline[i] = acc[i] / CALIB_SAMPLES;

    calib_save();
}

/* ── Init ── */
void matrix_init_custom(void) {
    EC_RCC_APB2ENR |= (1u<<2)|(1u<<3);
    for (int i=0;i<4;i++) { gpio_set_pin_output(mux_pins[i]); gpio_write_pin(mux_pins[i],0); }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 0);
    adc_init();

    wait_ms(50);

    /* Check if user is holding a key to force recalibration */
    bool force_calib = any_key_held();
    if (force_calib) wait_ms(300); /* let key settle */

    if (!force_calib && calib_load()) {
        /* Loaded from flash, ready instantly */
    } else {
        run_calibration();
    }

    memset(key_state, 0, sizeof(key_state));
    memset(last_change, 0, sizeof(last_change));
}

/* ── Scan ── */
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;
    uint32_t now = timer_read32();

    for (uint8_t col=0; col<COL_COUNT; col++) {
        for (uint8_t row=0; row<ROW_COUNT; row++) {
            uint16_t idx = row*COL_COUNT+col;
            uint16_t v = read_key(row, col);
            int16_t delta = (int16_t)v - (int16_t)baseline[idx];

            uint8_t want = key_state[idx];
            if (!key_state[idx] && delta >= PRESS_DELTA)
                want = 1;
            else if (key_state[idx] && delta <= RELEASE_DELTA)
                want = 0;

            if (want != key_state[idx]) {
                if (now - last_change[idx] >= DEBOUNCE_MS) {
                    key_state[idx] = want;
                    last_change[idx] = now;
                    changed = true;
                }
            } else {
                last_change[idx] = now;
            }

            /* Slow EMA drift compensation — only when key is released
             * and delta is small (not being pressed) */
            if (!key_state[idx] && delta > -20 && delta < 40) {
                baseline[idx] = baseline[idx]
                    + (int16_t)(((int32_t)(v - baseline[idx]) * DRIFT_ALPHA) >> 8);
            }
        }
    }

    for (uint8_t r=0;r<ROW_COUNT;r++) current_matrix[r]=0;
    for (int i=0;i<TOTAL_KEYS;i++) {
        if (key_state[i]) {
            uint8_t r = i / COL_COUNT;
            uint8_t c = i % COL_COUNT;
            current_matrix[r] |= (1u << c);
        }
    }
    return changed;
}
