/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio
    ChibiOS - Copyright (C) 2023..2025 HorrorTroll
    ChibiOS - Copyright (C) 2023..2025 Zhaqian

    Licensed under the Apache License, Version 2.0 (the "License");
*/

#include "hal.h"

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/**
 * @brief   PAL setup.
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
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Early initialization code.
 */
void __early_init(void) {
  at32_clock_init();
}

/**
 * @brief   Board-specific initialization code.
 */
void boardInit(void) {
  /* GPIO and matrix are configured by QMK at runtime. */
}
