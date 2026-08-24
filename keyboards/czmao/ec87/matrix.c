/* EC87 v4.2 — production capacitive sensing with debounce */
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

/* Tuning */
#define DISCHARGE_US    10
#define SETTLE_US       5
#define MUX_SETTLE_US   3
#define CALIB_SAMPLES   16
#define PRESS_DELTA     120
#define RELEASE_DELTA   80
#define DEBOUNCE_MS     5

static uint16_t baseline[TOTAL_KEYS];
static uint8_t  key_state[TOTAL_KEYS];
static uint32_t last_change[TOTAL_KEYS];

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
    __asm__ volatile ("" ::: "memory");
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

static void calibrate(void) {
    /* warm-up */
    for (uint8_t c=0;c<COL_COUNT;c++)
        for (uint8_t r=0;r<ROW_COUNT;r++)
            read_key(r, c);

    uint32_t acc[TOTAL_KEYS];
    memset(acc, 0, sizeof(acc));
    for (int s=0;s<CALIB_SAMPLES;s++)
        for (uint8_t c=0;c<COL_COUNT;c++)
            for (uint8_t r=0;r<ROW_COUNT;r++)
                acc[r*COL_COUNT+c] += read_key(r, c);

    for (int i=0;i<TOTAL_KEYS;i++)
        baseline[i] = acc[i] / CALIB_SAMPLES;
}

void matrix_init_custom(void) {
    EC_RCC_APB2ENR |= (1u<<2)|(1u<<3);
    for (int i=0;i<4;i++) { gpio_set_pin_output(mux_pins[i]); gpio_write_pin(mux_pins[i],0); }
    gpio_set_pin_output(MUX_EN_PIN);
    gpio_write_pin(MUX_EN_PIN, 0);
    adc_init();
    wait_ms(500);
    calibrate();
    memset(key_state, 0, sizeof(key_state));
    memset(last_change, 0, sizeof(last_change));
}

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
