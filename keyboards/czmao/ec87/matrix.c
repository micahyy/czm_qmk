/* EC87 diagnostic firmware v2 — repeating A/B/C/D/E tests + ADC sanity */
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
#define EC_ADC1_SMPR1   (*(volatile uint32_t *)0x4001240C)
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
    /* SMPR1: channels 10-17 — set ch16 & ch17 to 239.5 cycles */
    EC_ADC1_SMPR1 = 0x00FFFFFF;
    EC_ADC1_SQR1 = 0;
}

static uint16_t adc_read(uint8_t ch) {
    EC_ADC1_SQR3 = ch;
    EC_ADC1_CR2 |= (1u<<22);
    volatile uint32_t t=100000;
    while(!(EC_ADC1_SR&(1u<<1))&&--t);
    return (uint16_t)(EC_ADC1_DR & 0xFFFu);
}

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
    *cr = (*cr & ~(0xFu << sh)) | (0x8u << sh);
    if (pin < 16) EC_GPIOA_BSRR = (1u << (pin+16));
}

static uint16_t read_vrefint(void) {
    /* ch17 = VREFINT ~1.2V -> should read ~1489 */
    EC_ADC1_CR2 |= (1u<<23); /* TSVREFE: temp sensor & VREFINT enable */
    wait_ms(3);
    uint16_t v = adc_read(17);
    EC_ADC1_CR2 &= ~(1u<<23);
    return v;
}

static uint16_t read_temp(void) {
    EC_ADC1_CR2 |= (1u<<23);
    wait_ms(3);
    uint16_t v = adc_read(16);
    EC_ADC1_CR2 &= ~(1u<<23);
    return v;
}

void matrix_init_custom(void) {
    debug_enable = true;
    debug_matrix = true;

    EC_RCC_APB2ENR |= (1u<<2)|(1u<<3);
    for (int i=0;i<4;i++) { gpio_set_pin_output(mux_pins[i]); gpio_write_pin(mux_pins[i],0); }
    gpio_set_pin_output(MUX_EN_PIN);
    adc_init();

    print("\n=== EC87 DIAG v2 ===\n");
    /* Give console time to connect */
    print("Waiting 5s for console...\n");
    wait_ms(5000);
    print("Starting tests.\n");
    gpio_write_pin(MUX_EN_PIN, 0);
}

static void run_diag(void) {
    print("\n--- DIAG CYCLE ---\n");

    /* ADC sanity: VREFINT should be ~1490, temp ~1000-1800 */
    uint16_t vref = read_vrefint();
    uint16_t temp = read_temp();
    xprintf("ADC VREFINT(ch17)=%u  TEMP(ch16)=%u\n", vref, temp);

    mux_set(0);
    gpio_write_pin(MUX_EN_PIN, 0);

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        uint8_t pin = row + 2;
        uint8_t ch = row + 2;
        uint16_t v;

        /* A: PA1=LOW, row=analog -> expect ~0 */
        set_pin_output(1, 0);
        set_pin_analog(pin);
        wait_us(50);
        v = adc_read(ch);
        xprintf("R%d A(PA1=L,analog) =%u\n", row, v);

        /* B: PA1=HIGH, row=analog -> current scheme */
        set_pin_output(1, 1);
        wait_us(50);
        v = adc_read(ch);
        xprintf("R%d B(PA1=H,analog) =%u\n", row, v);

        /* C: PA1=HIGH, row=pull-down -> DC short test */
        set_pin_input_pulldown(pin);
        wait_us(50);
        v = adc_read(ch);
        xprintf("R%d C(PA1=H,pulldn) =%u\n", row, v);

        /* D: PA1=HIGH, row=output LOW -> must be 0 */
        set_pin_output(pin, 0);
        wait_us(50);
        v = adc_read(ch);
        xprintf("R%d D(PA1=H,OUT_L)  =%u\n", row, v);

        /* E: PA1=HIGH, mux OFF, row=analog -> no mux connection */
        gpio_write_pin(MUX_EN_PIN, 1);
        set_pin_analog(pin);
        set_pin_output(1, 1);
        wait_us(50);
        v = adc_read(ch);
        xprintf("R%d E(PA1=H,muxOFF) =%u\n", row, v);
        gpio_write_pin(MUX_EN_PIN, 0);
    }

    /* Restore safe state */
    set_pin_output(1, 0);
    for (uint8_t r=0;r<6;r++) set_pin_output(r+2, 0);
    print("--- END DIAG ---\n");
}

static uint16_t scan_count = 0;
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    for (uint8_t r=0;r<ROW_COUNT;r++) current_matrix[r]=0;

    if (++scan_count >= 500) {
        scan_count = 0;
        run_diag();
    }
    return false;
}
