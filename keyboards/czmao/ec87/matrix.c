/* EC87 v4.0 — fixed ADC register addresses + proper capacitive sensing */
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

/* ADC register map (STM32F103, base 0x40012400) */
#define EC_RCC_APB2ENR  (*(volatile uint32_t *)0x40021018)
#define EC_RCC_CFGR     (*(volatile uint32_t *)0x40021004)
#define EC_ADC1_SR      (*(volatile uint32_t *)0x40012400)
#define EC_ADC1_CR1     (*(volatile uint32_t *)0x40012404)
#define EC_ADC1_CR2     (*(volatile uint32_t *)0x40012408)
#define EC_ADC1_SMPR1   (*(volatile uint32_t *)0x4001240C)
#define EC_ADC1_SMPR2   (*(volatile uint32_t *)0x40012410)
/* SQR1 @ 0x2C, SQR2 @ 0x30, SQR3 @ 0x34, DR @ 0x4C */
#define EC_ADC1_SQR1    (*(volatile uint32_t *)0x4001242C)
#define EC_ADC1_SQR3    (*(volatile uint32_t *)0x40012434)
#define EC_ADC1_DR      (*(volatile uint32_t *)0x4001244C)

#define EC_GPIOA_CRL    (*(volatile uint32_t *)0x40010800)
#define EC_GPIOA_CRH    (*(volatile uint32_t *)0x40010804)
#define EC_GPIOA_BSRR   (*(volatile uint32_t *)0x40010810)
#define EC_GPIOA_ODR    (*(volatile uint32_t *)0x4001080C)

/* Timing parameters */
#define DISCHARGE_US    5
#define SETTLE_US       3
#define SAMPLE_DELAY_US 2

static void mux_set(uint8_t ch) {
    for (int i = 0; i < 4; i++)
        gpio_write_pin(mux_pins[i], (ch >> i) & 1);
}

static void adc_init(void) {
    EC_RCC_APB2ENR |= (1u<<2)|(1u<<3)|(1u<<9);  /* GPIOA, GPIOB, ADC1 */
    /* ADCCLK = PCLK2/6 = 72/6 = 12MHz (max 14MHz) */
    EC_RCC_CFGR = (EC_RCC_CFGR & ~(3u<<14)) | (2u<<14);

    /* Power on ADC */
    EC_ADC1_CR2 = (1u<<0);  /* ADON */
    wait_ms(2);             /* tSTAB */

    /* Correct calibration order: RSTCAL first, then CAL */
    EC_ADC1_CR2 |= (1u<<3);  /* RSTCAL */
    { volatile uint32_t t=200000; while((EC_ADC1_CR2&(1u<<3))&&--t); }
    EC_ADC1_CR2 |= (1u<<2);  /* CAL */
    { volatile uint32_t t=200000; while((EC_ADC1_CR2&(1u<<2))&&--t); }

    EC_ADC1_CR1 = 0;
    /* ADON | EXTTRIG | SWSTART(extsel=111) */
    EC_ADC1_CR2 = (1u<<0)|(1u<<20)|(7u<<17);
    /* 55.5 cycles sample time for all channels */
    EC_ADC1_SMPR2 = 0x2DB6DB6D;
    EC_ADC1_SMPR1 = 0x00FFFFFF;
    EC_ADC1_SQR1 = 0;  /* 1 conversion in regular sequence */
}

static uint16_t adc_read(uint8_t ch) {
    EC_ADC1_SQR3 = ch & 0x1F;  /* SQ1 = channel */
    EC_ADC1_CR2 |= (1u<<22);   /* SWSTART */
    volatile uint32_t t=100000;
    while(!(EC_ADC1_SR&(1u<<1))&&--t);  /* wait EOC */
    return (uint16_t)(EC_ADC1_DR & 0xFFFu);
}

/* GPIO helpers */
static void set_pin_output(uint8_t pin, uint8_t high) {
    uint8_t sh = pin * 4;
    volatile uint32_t *cr = (pin < 8) ? &EC_GPIOA_CRL : &EC_GPIOA_CRH;
    if (pin >= 8) sh -= 32;
    uint32_t mask = 0xFu << sh;
    *cr = (*cr & ~mask) | (0x3u << sh);  /* CNF=00, MODE=11 = output 50MHz push-pull */
    if (high) EC_GPIOA_BSRR = (1u << pin);
    else      EC_GPIOA_BSRR = (1u << (pin+16));
}

static void set_pin_analog(uint8_t pin) {
    uint8_t sh = pin * 4;
    volatile uint32_t *cr = (pin < 8) ? &EC_GPIOA_CRL : &EC_GPIOA_CRH;
    if (pin >= 8) sh -= 32;
    *cr &= ~(0xFu << sh);  /* CNF=00, MODE=00 = analog input */
}

void matrix_init_custom(void) {
    debug_enable = true;
    debug_matrix = true;

    EC_RCC_APB2ENR |= (1u<<2)|(1u<<3);
    for (int i=0;i<4;i++) { gpio_set_pin_output(mux_pins[i]); gpio_write_pin(mux_pins[i],0); }
    gpio_set_pin_output(MUX_EN_PIN);
    adc_init();

    print("\n=== EC87 v4.0 ===\n");

    /* ADC sanity check: VREFINT ~1.2V should read ~1489, not 4082 */
    EC_ADC1_CR2 |= (1u<<23);  /* TSVREFE */
    wait_ms(3);
    uint16_t vref = adc_read(17);
    uint16_t temp = adc_read(16);
    EC_ADC1_CR2 &= ~(1u<<23);
    xprintf("ADC VREFINT(ch17)=%u  TEMP(ch16)=%u\n", vref, temp);
    xprintf("VREFINT expected ~1400-1600, if 4082 ADC still broken\n");

    gpio_write_pin(MUX_EN_PIN, 0);

    /* Quick test: read PA1 with LOW and HIGH */
    set_pin_output(1, 0);
    set_pin_analog(1);
    wait_us(50);
    uint16_t pa1_low = adc_read(1);
    set_pin_output(1, 1);
    wait_us(50);
    uint16_t pa1_high = adc_read(1);
    xprintf("PA1 self-test: LOW=%u HIGH=%u\n", pa1_low, pa1_high);

    set_pin_output(1, 0);
    wait_ms(1000);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    for (uint8_t col = 0; col < COL_COUNT; col++) {
        mux_set(col);

        /* Phase 1: discharge — PA1=LOW, all rows=LOW */
        set_pin_output(1, 0);
        for (uint8_t r=0;r<ROW_COUNT;r++) set_pin_output(r+2, 0);
        wait_us(DISCHARGE_US);

        for (uint8_t row = 0; row < ROW_COUNT; row++) {
            uint8_t pin = row + 2;
            uint8_t ch = row + 2;

            /* Phase 2: target row floats, other rows stay LOW (shield) */
            set_pin_analog(pin);

            /* Phase 3: PA1 voltage step, coupled through C_key */
            set_pin_output(1, 1);
            wait_us(SETTLE_US);

            /* Phase 4: ADC read */
            uint16_t v = adc_read(ch);

            /* Phase 5: discharge */
            set_pin_output(1, 0);
            set_pin_output(pin, 0);

            /* Threshold: pressed key couples more charge → higher voltage */
            /* COM has 1M pull-down, so resting voltage should be low (~0) */
            if (v > 200) {
                current_matrix[row] |= (1u << col);
                changed = true;
            }

            (void)SAMPLE_DELAY_US;
        }
    }

    return changed;
}
