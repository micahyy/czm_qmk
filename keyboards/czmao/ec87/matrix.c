/* EC87 diagnostic firmware — test GPIO/ADC configurations to locate DC path */
#include "matrix.h"
#include "gpio.h"
#include "wait.h"
#include "print.h"
#include "debug.h"
#include <stdint.h>

#define ROW_COUNT 6
#define COL_COUNT 16

static const pin_t mux_pins[4] = {B8, B9, B10, B11};
#define MUX_EN_PIN B12

#define EC_RCC_APB2ENR  (*(volatile uint32_t *)0x40021018)
#define EC_RCC_CFGR     (*(volatile uint32_t *)0x40021004)
#define EC_ADC1_SR      (*(volatile uint32_t *)0x40012400)
#define EC_ADC1_CR1     (*(volatile uint32_t *)0x40012404)
#define EC_ADC1_CR2     (*(volatile uint32_t *)0x40012408)
#define EC_ADC1_SMPR2   (*(volatile uint32_t *)0x40012410)
#define EC_ADC1_SQR1    (*(volatile uint32_t *)0x40012428)
#define EC_ADC1_SQR3    (*(volatile uint32_t *)0x4001242C)
#define EC_ADC1_DR      (*(volatile uint32_t *)0x4001244C)
#define EC_GPIOA_CRL    (*(volatile uint32_t *)0x40010800)
#define EC_GPIOA_CRH    (*(volatile uint32_t *)0x40010804)
#define EC_GPIOA_BSRR   (*(volatile uint32_t *)0x40010810)
#define EC_GPIOA_ODR    (*(volatile uint32_t *)0x4001080C)

static void mux_set(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

static void adc_init(void) {
    EC_RCC_APB2ENR |= (1u<<2)|(1u<<3)|(1u<<9);
    EC_RCC_CFGR = (EC_RCC_CFGR & ~(3u<<14)) | (2u<<14);
    EC_ADC1_CR2 = (1u<<0); wait_ms(2);
    EC_ADC1_CR2 |= (1u<<3);
    { volatile uint32_t t=50000; while((EC_ADC1_CR2&(1u<<3))&&--t); }
    EC_ADC1_CR2 |= (1u<<2);
    { volatile uint32_t t=200000; while((EC_ADC1_CR2&(1u<<2))&&--t); }
    EC_ADC1_CR1 = 0;
    EC_ADC1_CR2 = (1u<<0)|(1u<<20)|(7u<<17);
    EC_ADC1_SMPR2 = 0x2DB6DB6D;
    EC_ADC1_SQR1 = 0;
}

static uint16_t adc_read(uint8_t ch) {
    EC_ADC1_SQR3 = ch;
    EC_ADC1_CR2 |= (1u<<22);
    volatile uint32_t t=100000;
    while(!(EC_ADC1_SR&(1u<<1))&&--t);
    return (uint16_t)(EC_ADC1_DR & 0xFFFu);
}

/* GPIO helpers */
static void set_pin_output(uint8_t pin, uint8_t high) {
    uint8_t sh = pin * 4;
    volatile uint32_t *cr = (pin < 8) ? &EC_GPIOA_CRL : &EC_GPIOA_CRH;
    if (pin >= 8) sh -= 32;
    uint32_t mask = 0xFu << sh;
    *cr = (*cr & ~mask) | (0x3u << sh);
    if (high) EC_GPIOA_BSRR = (1u << pin);
    else      EC_GPIOA_BSRR = (1u << (pin+16));
}

static void set_pin_analog(uint8_t pin) {
    uint8_t sh = pin * 4;
    volatile uint32_t *cr = (pin < 8) ? &EC_GPIOA_CRL : &EC_GPIOA_CRH;
    if (pin >= 8) sh -= 32;
    *cr = (*cr & ~(0xFu << sh));
}

static void set_pin_input_pulldown(uint8_t pin) {
    uint8_t sh = pin * 4;
    volatile uint32_t *cr = (pin < 8) ? &EC_GPIOA_CRL : &EC_GPIOA_CRH;
    if (pin >= 8) sh -= 32;
    *cr = (*cr & ~(0xFu << sh)) | (0x8u << sh); // CNF=10, MODE=00
    if (pin < 16) EC_GPIOA_BSRR = (1u << (pin+16)); // ODR=0 = pulldown
}

void matrix_init_custom(void) {
    debug_enable = true;
    debug_matrix = true;

    print("\n=== EC87 DIAGNOSTIC v3.9d ===\n");

    EC_RCC_APB2ENR |= (1u<<2)|(1u<<3);
    for (int i=0;i<4;i++) { gpio_set_pin_output(mux_pins[i]); gpio_write_pin(mux_pins[i],0); }
    gpio_set_pin_output(MUX_EN_PIN);
    adc_init();
    print("ADC ready\n");

    gpio_write_pin(MUX_EN_PIN, 0); // enable mux

    /* Test each row for col 0 */
    print("\n--- Test on COL 0 ---\n");
    mux_set(0);

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        uint8_t pin = row + 2;
        uint8_t ch = row + 2;
        uint16_t v;

        /* A: pin analog, PA1=LOW -> expect ~0 */
        set_pin_output(1, 0);
        set_pin_analog(pin);
        wait_us(10);
        v = adc_read(ch);
        xprintf("R%d A(PA1=L,row=analog): %u\n", row, v);

        /* B: pin analog, PA1=HIGH -> current scheme */
        set_pin_output(1, 1);
        wait_us(10);
        v = adc_read(ch);
        xprintf("R%d B(PA1=H,row=analog): %u\n", row, v);

        /* C: pin PULL-DOWN, PA1=HIGH -> if DC short, still high;
           if only cap coupling, should read ~0 */
        set_pin_input_pulldown(pin);
        wait_us(10);
        v = adc_read(ch);
        xprintf("R%d C(PA1=H,row=PD)    : %u\n", row, v);

        /* D: pin output LOW, PA1=HIGH -> driven low, should be 0 */
        set_pin_output(pin, 0);
        wait_us(10);
        v = adc_read(ch);
        xprintf("R%d D(PA1=H,row=OUT_L) : %u\n", row, v);

        /* E: pin analog, PA1=HIGH, mux DISABLED -> should be 0 (no connection) */
        gpio_write_pin(MUX_EN_PIN, 1);
        set_pin_analog(pin);
        set_pin_output(1, 1);
        wait_us(10);
        v = adc_read(ch);
        xprintf("R%d E(PA1=H,mux=OFF)   : %u\n", row, v);
        gpio_write_pin(MUX_EN_PIN, 0);
    }

    /* Also test: all rows analog, PA1=LOW, read each -> floating noise */
    print("\n--- All rows analog, PA1=LOW ---\n");
    set_pin_output(1, 0);
    for (uint8_t r=0;r<6;r++) set_pin_analog(r+2);
    wait_us(50);
    for (uint8_t r=0;r<6;r++) {
        xprintf("R%d: %u  ", r, adc_read(r+2));
    }
    print("\n");

    /* Test: all rows analog, PA1=HIGH, mux OFF */
    print("--- All rows analog, PA1=HIGH, mux OFF ---\n");
    gpio_write_pin(MUX_EN_PIN, 1);
    set_pin_output(1, 1);
    wait_us(50);
    for (uint8_t r=0;r<6;r++) {
        xprintf("R%d: %u  ", r, adc_read(r+2));
    }
    print("\n");

    gpio_write_pin(MUX_EN_PIN, 0);
    print("\n=== DIAG DONE ===\n\n");

    // Keep printing repeating B test for live debugging
}

static uint8_t scan_cycle = 0;
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    for (uint8_t r=0;r<ROW_COUNT;r++) current_matrix[r]=0;

    // After initial diag, just do a simple continuous read of B config
    if (++scan_cycle >= 30) {
        scan_cycle = 0;
        gpio_write_pin(MUX_EN_PIN, 0);
        mux_set(0);
        set_pin_output(1, 1);
        print("LIVE col0:");
        for (uint8_t r=0;r<6;r++) {
            set_pin_analog(r+2);
            wait_us(5);
            xprintf(" R%d=%u", r, adc_read(r+2));
            set_pin_output(r+2, 0);
        }
        print("\n");
        set_pin_output(1, 0);
    }
    return false;
}
