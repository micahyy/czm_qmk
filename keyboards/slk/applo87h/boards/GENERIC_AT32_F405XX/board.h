/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio
    ChibiOS - Copyright (C) 2023..2026 HorrorTroll
    ChibiOS - Copyright (C) 2023..2026 Zhaqian
    ChibiOS - Copyright (C) 2024..2026 Maxjta

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for SLK Applo87H — AT32F405RCT7 (LQFP64).
 */

/*
 * Board identifier.
 */
#define BOARD_GENERIC_AT32_F405XX
#define BOARD_NAME                  "SLK Applo87H AT32F405"

/*
 * Board oscillators-related settings.
 * 12 MHz HEXT crystal on PF0/PF1; 32.768 kHz LEXT on PC14/PC15.
 */
#if !defined(AT32_LEXTCLK)
#define AT32_LEXTCLK                32768
#endif

#if !defined(AT32_HEXTCLK)
#define AT32_HEXTCLK                12000000
#endif

/*
 * MCU type: AT32F405RC = LQFP64, 256KB Flash, High Density.
 */
#define AT32F405RC

/*
 * GPIO availability for LQFP64 package.
 */
#define AT32_HAS_GPIOC              TRUE
#define AT32_HAS_GPIOD              TRUE
#define AT32_HAS_GPIOF              TRUE

/*
 * CRITICAL: Enable OTG2 (OTGHS) High-Speed on-chip PHY.
 *
 * On AT32F405 this macro tells the OTGv1 USB LLD to:
 *   - call crmEnableOTG_HS() (OTGHS clock path)
 *   - NOT set GUSBCFG.PHYSEL (select on-chip HS PHY)
 *   - set DCFG.DEVSPD = HS (480 Mbps) when AT32_USE_USB_OTG2_HS=TRUE
 */
#define BOARD_OTG2_USES_ULPI

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

/* Port D: only PD2 is bonded on LQFP64 (Scroll Lock LED). */
#define GPIOD_PIN2                  2U

/* Port F: PF0/PF1 = HEXT crystal, PF11 = BOOT0. */
#define GPIOF_HEXT_IN               0U
#define GPIOF_HEXT_OUT              1U
#define GPIOF_PIN4                  4U
#define GPIOF_PIN5                  5U
#define GPIOF_PIN6                  6U
#define GPIOF_PIN7                  7U
#define GPIOF_PIN11                 11U

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*
 * I/O ports initial setup.
 *
 * Configuration digit values:
 *   0 - Analog input
 *   4 - Digital input (floating)
 *   8 - Digital input with pull-up/pull-down
 */

/*
 * Port A: all inputs with pull-up.
 * PA13/PA14 (SWD) are reconfigured by hardware after reset.
 */
#define VAL_GPIOACFGLR          0x88888888      /*  PA7...PA0 */
#define VAL_GPIOACFGHR          0x88888888      /* PA15...PA8 */
#define VAL_GPIOAODT            0xFFFFFFFF

/*
 * Port B: all inputs with pull-up.
 * PB13 = OTGHS VBUS sense (board pull-up).
 * PB14/PB15 are not bonded on AT32F405 LQFP64; writes are harmless.
 */
#define VAL_GPIOBCFGLR          0x88888888      /*  PB7...PB0 */
#define VAL_GPIOBCFGHR          0x88888888      /* PB15...PB8 */
#define VAL_GPIOBODT            0xFFFFFFFF

/*
 * Port C: all inputs with pull-up.
 * PC14/PC15 = LEXT (32.768 kHz), handled by ERTC hardware.
 */
#define VAL_GPIOCCFGLR          0x88888888      /*  PC7...PC0 */
#define VAL_GPIOCCFGHR          0x88888888      /* PC15...PC8 */
#define VAL_GPIOCODT            0xFFFFFFFF

/*
 * Port D: only PD2 is bonded (Scroll Lock LED, push-pull output).
 * PD0/PD1 do not exist on LQFP64; set to analog.
 */
#define VAL_GPIODCFGLR          0x88888800      /*  PD7...PD0: PD0/1=analog, PD2..7=pull-up */
#define VAL_GPIODCFGHR          0x88888888      /* PD15...PD8 */
#define VAL_GPIODODT            0xFFFFFFFF

/*
 * Port F: PF0/PF1 = HEXT crystal (analog mode).
 * PF4-PF7 = pull-up inputs, PF11 = BOOT0.
 */
#define VAL_GPIOFCFGLR          0x88880000      /*  PF7...PF0: PF0/1=analog, PF4..7=pull-up */
#define VAL_GPIOFCFGHR          0x88888888      /* PF15...PF8 */
#define VAL_GPIOFODT            0xFFFFFFFF

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
