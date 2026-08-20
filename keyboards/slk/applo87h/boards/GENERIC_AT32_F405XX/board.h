/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio
    ChibiOS - Copyright (C) 2023..2025 HorrorTroll
    ChibiOS - Copyright (C) 2023..2025 Zhaqian

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Board identifier.
 */
#define BOARD_GENERIC_AT32_F405XX
#define BOARD_NAME                  "SLK Applo87H AT32F405"

/*
 * Board oscillators-related settings.
 */
#if !defined(AT32_LEXTCLK)
#define AT32_LEXTCLK                32768
#endif

#if !defined(AT32_HEXTCLK)
#define AT32_HEXTCLK                12000000
#endif

/*
 * MCU type: AT32F405RC = LQFP64, 256KB Flash.
 */
#define AT32F405RC

/*
 * GPIO settings.
 */
#if defined(AT32F405KB) || defined(AT32F405KC) || defined(AT32F405CB) || \
    defined(AT32F405CC) || defined(AT32F405RB) || defined(AT32F405RC)
#define AT32_HAS_GPIOC              TRUE
#define AT32_HAS_GPIOF              TRUE
#endif

/*
 * IO pins assignments.
 */
#define GPIOA_PIN0                  0U
#define GPIOA_PIN1                  1U
#define GPIOA_PIN2                  2U
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
#define GPIOB_PIN13                 13U
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
#define GPIOD_PIN2                  2U

#define GPIOF_PIN0                  0U
#define GPIOF_PIN1                  1U

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

/*
 * GPIO port configuration.
 * All pins set to input pull-up/analog (safe defaults).
 * QMK configures matrix pins at runtime.
 */
#define VAL_GPIOAODT            0xFFFFFFFF
#define VAL_GPIOACFGLR          0x88888888
#define VAL_GPIOACFGHR          0x88888888

#define VAL_GPIOBODT            0xFFFFFFFF
#define VAL_GPIOBCFGLR          0x88888888
#define VAL_GPIOBCFGHR          0x88888888

#define VAL_GPIOCODT            0xFFFFFFFF
#define VAL_GPIOCCFGLR          0x88888888
#define VAL_GPIOCCFGHR          0x88888888

#define VAL_GPIODODT            0xFFFFFFFF
#define VAL_GPIODCFGLR          0x88888888
#define VAL_GPIODCFGHR          0x88888888

#define VAL_GPIOFODT            0xFFFFFFFF
#define VAL_GPIOFCFGLR          0x88880000
#define VAL_GPIOFCFGHR          0x88888888

#endif /* _BOARD_H_ */
