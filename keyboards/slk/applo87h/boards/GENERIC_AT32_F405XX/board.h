/*
    ChibiOS - Copyright (C) 2006..2020 Giovanni Di Sirio
    ChibiOS - Copyright (C) 2023..2025 HorrorTroll
    ChibiOS - Copyright (C) 2023..2025 Zhaqian
    ChibiOS - Copyright (C) 2024..2025 Maxjta

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#define BOARD_GENERIC_AT32_F405XX
#define BOARD_NAME                  "SLK Applo87H AT32F405"

#if !defined(AT32_LEXTCLK)
#define AT32_LEXTCLK                32768
#endif

#if !defined(AT32_HEXTCLK)
#define AT32_HEXTCLK                12000000
#endif

#define AT32F405RC

/*
 * IO pin assignments. Values are only used by VAL_* macros below.
 */
#define GPIOA_PIN0                  0U
#define GPIOA_PIN1                  1U
#define GPIOA_LED_CAPS              2U
#define GPIOA_PIN3                  3U
#define GPIOA_PIN4                  4U
#define GPIOA_PIN5                  5U
#define GPIOA_PIN6                  6U
#define GPIOA_PIN7                  7U
#define GPIOA_PIN8                  8U
#define GPIOA_PIN9                  9U
#define GPIOA_PIN10                 10U
#define GPIOA_PIN11                 11U
#define GPIOA_PIN12                 12U
#define GPIOA_SWDIO                 13U
#define GPIOA_SWCLK                 14U
#define GPIOA_PIN15                 15U

#define GPIOB_PIN0                  0U
#define GPIOB_PIN1                  1U
#define GPIOB_PIN2                  2U
#define GPIOB_PIN3                  3U
#define GPIOB_PIN4                  4U
#define GPIOB_PIN5                  5U
#define GPIOB_PIN6                  6U
#define GPIOB_PIN7                  7U
#define GPIOB_PIN8                  8U
#define GPIOB_PIN9                  9U
#define GPIOB_PIN10                 10U
#define GPIOB_PIN11                 11U
#define GPIOB_PIN12                 12U
#define GPIOB_USB_VBUS              13U
#define GPIOB_PIN14                 14U
#define GPIOB_PIN15                 15U

#define GPIOC_PIN0                  0U
#define GPIOC_PIN1                  1U
#define GPIOC_PIN2                  2U
#define GPIOC_PIN3                  3U
#define GPIOC_PIN4                  4U
#define GPIOC_PIN5                  5U
#define GPIOC_PIN6                  6U
#define GPIOC_PIN7                  7U
#define GPIOC_PIN8                  8U
#define GPIOC_PIN9                  9U
#define GPIOC_PIN10                 10U
#define GPIOC_PIN11                 11U
#define GPIOC_PIN12                 12U
#define GPIOC_PIN13                 13U
#define GPIOC_PIN14                 14U
#define GPIOC_PIN15                 15U

#define GPIOD_PIN0                  0U
#define GPIOD_PIN1                  1U
#define GPIOD_LED_SCROLL            2U

#define GPIOF_HEXT_IN               0U
#define GPIOF_HEXT_OUT              1U

/* Input/mode/value macros matching AT32 F402/F405 GPIOv2. */
#define PIN_MODE_INPUT(n)           (0U << ((n) * 2U))
#define PIN_MODE_OUTPUT(n)          (1U << ((n) * 2U))
#define PIN_MODE_MUX(n)             (2U << ((n) * 2U))
#define PIN_MODE_ANALOG(n)          (3U << ((n) * 2U))
#define PIN_ODT_LOW(n)              (0U << (n))
#define PIN_ODT_HIGH(n)             (1U << (n))
#define PIN_OMODE_PUSHPULL(n)       (0U << (n))
#define PIN_OMODE_OPENDRAIN(n)      (1U << (n))
#define PIN_ODRVR_STRONGER(n)       (1U << ((n) * 2U))
#define PIN_ODRVR_MODERATE(n)       (3U << ((n) * 2U))
#define PIN_PULL_FLOATING(n)        (0U << ((n) * 2U))
#define PIN_PULL_PULLUP(n)          (1U << ((n) * 2U))
#define PIN_PULL_PULLDOWN(n)        (2U << ((n) * 2U))
#define PIN_IOMUX_MUX(n, v)         ((v) << (((n) % 8U) * 4U))
#define PIN_HDRV_DISABLED(n)        (0U << (n))
#define PIN_HDRV_ENABLED(n)         (1U << (n))

/*
 * Safe reset defaults. Matrix rows/columns and LEDs are reconfigured by QMK.
 * PA13/PA14 keep SWD; PF0/PF1 keep HEXT; PB13 is VBUS sense input.
 */
#define VAL_GPIOA_CFGR (PIN_MODE_INPUT(GPIOA_PIN0) | PIN_MODE_INPUT(GPIOA_PIN1) | \
  PIN_MODE_INPUT(GPIOA_LED_CAPS) | PIN_MODE_INPUT(GPIOA_PIN3) | PIN_MODE_INPUT(GPIOA_PIN4) | \
  PIN_MODE_INPUT(GPIOA_PIN5) | PIN_MODE_INPUT(GPIOA_PIN6) | PIN_MODE_INPUT(GPIOA_PIN7) | \
  PIN_MODE_INPUT(GPIOA_PIN8) | PIN_MODE_INPUT(GPIOA_PIN9) | PIN_MODE_INPUT(GPIOA_PIN10) | \
  PIN_MODE_INPUT(GPIOA_PIN11) | PIN_MODE_INPUT(GPIOA_PIN12) | PIN_MODE_MUX(GPIOA_SWDIO) | \
  PIN_MODE_MUX(GPIOA_SWCLK) | PIN_MODE_INPUT(GPIOA_PIN15))
#define VAL_GPIOA_OMODE 0
#define VAL_GPIOA_ODRVR 0
#define VAL_GPIOA_PULL (PIN_PULL_PULLUP(GPIOA_PIN0) | PIN_PULL_PULLUP(GPIOA_PIN1) | \
  PIN_PULL_PULLUP(GPIOA_LED_CAPS) | PIN_PULL_PULLUP(GPIOA_PIN3) | PIN_PULL_PULLUP(GPIOA_PIN4) | \
  PIN_PULL_PULLUP(GPIOA_PIN5) | PIN_PULL_PULLUP(GPIOA_PIN6) | PIN_PULL_PULLUP(GPIOA_PIN7) | \
  PIN_PULL_PULLUP(GPIOA_PIN8) | PIN_PULL_PULLUP(GPIOA_PIN9) | PIN_PULL_PULLUP(GPIOA_PIN10) | \
  PIN_PULL_PULLUP(GPIOA_PIN11) | PIN_PULL_PULLUP(GPIOA_PIN12) | PIN_PULL_PULLUP(GPIOA_SWDIO) | \
  PIN_PULL_PULLDOWN(GPIOA_SWCLK) | PIN_PULL_PULLUP(GPIOA_PIN15))
#define VAL_GPIOA_ODT 0xFFFFFFFFU
#define VAL_GPIOA_MUXL 0
#define VAL_GPIOA_MUXH 0
#define VAL_GPIOA_HDRV 0

#define VAL_GPIOB_CFGR (PIN_MODE_INPUT(GPIOB_PIN0) | PIN_MODE_INPUT(GPIOB_PIN1) | \
  PIN_MODE_INPUT(GPIOB_PIN2) | PIN_MODE_INPUT(GPIOB_PIN3) | PIN_MODE_INPUT(GPIOB_PIN4) | \
  PIN_MODE_INPUT(GPIOB_PIN5) | PIN_MODE_INPUT(GPIOB_PIN6) | PIN_MODE_INPUT(GPIOB_PIN7) | \
  PIN_MODE_INPUT(GPIOB_PIN8) | PIN_MODE_INPUT(GPIOB_PIN9) | PIN_MODE_INPUT(GPIOB_PIN10) | \
  PIN_MODE_INPUT(GPIOB_PIN11) | PIN_MODE_INPUT(GPIOB_PIN12) | PIN_MODE_INPUT(GPIOB_USB_VBUS) | \
  PIN_MODE_INPUT(GPIOB_PIN14) | PIN_MODE_INPUT(GPIOB_PIN15))
#define VAL_GPIOB_OMODE 0
#define VAL_GPIOB_ODRVR 0
#define VAL_GPIOB_PULL (PIN_PULL_PULLUP(GPIOB_PIN0) | PIN_PULL_PULLUP(GPIOB_PIN1) | \
  PIN_PULL_PULLUP(GPIOB_PIN2) | PIN_PULL_PULLUP(GPIOB_PIN3) | PIN_PULL_PULLUP(GPIOB_PIN4) | \
  PIN_PULL_PULLUP(GPIOB_PIN5) | PIN_PULL_PULLUP(GPIOB_PIN6) | PIN_PULL_PULLUP(GPIOB_PIN7) | \
  PIN_PULL_PULLUP(GPIOB_PIN8) | PIN_PULL_PULLUP(GPIOB_PIN9) | PIN_PULL_PULLUP(GPIOB_PIN10) | \
  PIN_PULL_PULLUP(GPIOB_PIN11) | PIN_PULL_PULLUP(GPIOB_PIN12) | PIN_PULL_FLOATING(GPIOB_USB_VBUS) | \
  PIN_PULL_PULLUP(GPIOB_PIN14) | PIN_PULL_PULLUP(GPIOB_PIN15))
#define VAL_GPIOB_ODT 0xFFFFFFFFU
#define VAL_GPIOB_MUXL 0
#define VAL_GPIOB_MUXH 0
#define VAL_GPIOB_HDRV 0

#define VAL_GPIOC_CFGR 0
#define VAL_GPIOC_OMODE 0
#define VAL_GPIOC_ODRVR 0
#define VAL_GPIOC_PULL 0xFFFFFFFFU
#define VAL_GPIOC_ODT 0xFFFFFFFFU
#define VAL_GPIOC_MUXL 0
#define VAL_GPIOC_MUXH 0
#define VAL_GPIOC_HDRV 0

#define VAL_GPIOD_CFGR (PIN_MODE_INPUT(GPIOD_PIN0) | PIN_MODE_INPUT(GPIOD_PIN1) | PIN_MODE_INPUT(GPIOD_LED_SCROLL))
#define VAL_GPIOD_OMODE 0
#define VAL_GPIOD_ODRVR 0
#define VAL_GPIOD_PULL (PIN_PULL_PULLUP(GPIOD_PIN0) | PIN_PULL_PULLUP(GPIOD_PIN1) | PIN_PULL_PULLUP(GPIOD_LED_SCROLL))
#define VAL_GPIOD_ODT 0xFFFFFFFFU
#define VAL_GPIOD_MUXL 0
#define VAL_GPIOD_MUXH 0
#define VAL_GPIOD_HDRV 0

#define VAL_GPIOF_CFGR (PIN_MODE_INPUT(GPIOF_HEXT_IN) | PIN_MODE_INPUT(GPIOF_HEXT_OUT))
#define VAL_GPIOF_OMODE 0
#define VAL_GPIOF_ODRVR 0
#define VAL_GPIOF_PULL (PIN_PULL_FLOATING(GPIOF_HEXT_IN) | PIN_PULL_FLOATING(GPIOF_HEXT_OUT))
#define VAL_GPIOF_ODT 0xFFFFFFFFU
#define VAL_GPIOF_MUXL 0
#define VAL_GPIOF_MUXH 0
#define VAL_GPIOF_HDRV 0

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif

#endif /* _BOARD_H_ */
