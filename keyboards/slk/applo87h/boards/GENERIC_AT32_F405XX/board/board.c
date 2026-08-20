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

#include "hal.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
#if HAL_USE_PAL || defined(__DOXYGEN__)
const PALConfig pal_default_config =
{
  {VAL_GPIOAODT, VAL_GPIOACFGLR, VAL_GPIOACFGHR},
  {VAL_GPIOBODT, VAL_GPIOBCFGLR, VAL_GPIOBCFGHR},
#if AT32_HAS_GPIOC
  {VAL_GPIOCODT, VAL_GPIOCCFGLR, VAL_GPIOCCFGHR},
#endif
  {VAL_GPIODODT, VAL_GPIODCFGLR, VAL_GPIODCFGHR},
#if AT32_HAS_GPIOF
  {VAL_GPIOFODT, VAL_GPIOFCFGLR, VAL_GPIOFCFGHR},
#endif
};
#endif

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Early initialization code.
 * @details System clocks are initialized before everything else.
 *          For AT32F405 this configures:
 *            HEXT  = 12 MHz
 *            PLL   : MS=1, NS=72, FP=4  -> SYSCLK = 216 MHz
 *            PLLU  : FU=18             -> USB clock = 48 MHz (OTGHS PHY)
 *            AHB   = 216 MHz, APB1 = 108 MHz, APB2 = 108 MHz
 */
void __early_init(void) {
  at32_clock_init();
}

#if HAL_USE_SDC || defined(__DOXYGEN__)
/**
 * @brief   SDC card detection.
 */
bool sdc_lld_is_card_inserted(SDCDriver *sdcp) {
  (void)sdcp;
  return true;
}

/**
 * @brief   SDC card write protection detection.
 */
bool sdc_lld_is_write_protected(SDCDriver *sdcp) {
  (void)sdcp;
  return false;
}
#endif /* HAL_USE_SDC */

/**
 * @brief   Board-specific initialization code.
 * @note    Disable JTAG to free PA15 (Col 0) for matrix use.
 *          PB3/PB4 are not used as matrix pins on this board but are
 *          also freed. SWD (PA13/PA14) remains functional for debugging.
 */
void boardInit(void) {
  IOMUX->REMAP |= IOMUX_REMAP_SWJTAG_MUX_JTAGDIS;
}
