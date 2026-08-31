/*
    Copyright (C) Zhaqian

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

/**
 * @file    AT32F403_7xx/at32_isr.c
 * @brief   AT32F403_7xx ISR handler code.
 *
 * @addtogroup AT32F403_7xx_ISR
 * @{
 */

#include "hal.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#define exti_serve_irq(pr, channel) {                                       \
                                                                            \
  if ((pr) & (1U << (channel))) {                                           \
    _pal_isr_code(channel);                                                 \
  }                                                                         \
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if (HAL_USE_PAL && (PAL_USE_WAIT || PAL_USE_CALLBACKS)) || defined(__DOXYGEN__)
#if !defined(AT32_DISABLE_EXINT0_HANDLER)
/**
 * @brief   EXINT[0] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT0_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & (1U << 0);
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 0);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT1_HANDLER)
/**
 * @brief   EXINT[1] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT1_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & (1U << 1);
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 1);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT2_HANDLER)
/**
 * @brief   EXINT[2] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT2_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & (1U << 2);
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 2);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT3_HANDLER)
/**
 * @brief   EXINT[3] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT3_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & (1U << 3);
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 3);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT4_HANDLER)
/**
 * @brief   EXINT[4] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT4_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & (1U << 4);
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 4);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT5_9_HANDLER)
/**
 * @brief   EXINT[5]...EXINT[9] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT5_9_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & ((1U << 5) | (1U << 6) | (1U << 7) | (1U << 8) | (1U << 9));
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 5);
  exti_serve_irq(pr, 6);
  exti_serve_irq(pr, 7);
  exti_serve_irq(pr, 8);
  exti_serve_irq(pr, 9);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT10_15_HANDLER)
/**
 * @brief   EXINT[10]...EXINT[15] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT10_15_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & ((1U << 10) | (1U << 11) | (1U << 12) | (1U << 13) | (1U << 14) | (1U << 15));
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 10);
  exti_serve_irq(pr, 11);
  exti_serve_irq(pr, 12);
  exti_serve_irq(pr, 13);
  exti_serve_irq(pr, 14);
  exti_serve_irq(pr, 15);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT16_HANDLER)
/**
 * @brief   EXINT[16] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT16_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & (1U << 16);
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 16);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT17_HANDLER)
/**
 * @brief   EXINT[17] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT17_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & (1U << 17);
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 17);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT18_HANDLER)
/**
 * @brief   EXINT[18] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT18_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & (1U << 18);
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 18);

  OSAL_IRQ_EPILOGUE();
}
#endif

#if !defined(AT32_DISABLE_EXINT19_HANDLER)
/**
 * @brief   EXINT[19] interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EXINT19_HANDLER) {
  uint32_t pr;

  OSAL_IRQ_PROLOGUE();

  pr = EXINT->INTSTS & (1U << 19);
  EXINT->INTSTS = pr;

  exti_serve_irq(pr, 19);

  OSAL_IRQ_EPILOGUE();
}
#endif
#endif

#include "at32_tmr1_9_10_11.inc"
#include "at32_tmr2.inc"
#include "at32_tmr3.inc"
#include "at32_tmr4.inc"
#include "at32_tmr5.inc"
#include "at32_tmr6.inc"
#include "at32_tmr7.inc"
#include "at32_tmr8_12_13_14.inc"

#include "at32_usart1.inc"
#include "at32_usart2.inc"
#include "at32_usart3.inc"
#include "at32_uart4.inc"
#include "at32_uart5.inc"
#include "at32_usart6.inc"
#include "at32_uart7.inc"
#include "at32_uart8.inc"

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Enables IRQ sources.
 *
 * @notapi
 */
void irqInit(void) {

#if HAL_USE_PAL
  nvicEnableVector(AT32_EXINT0_NUMBER, AT32_IRQ_EXINT0_PRIORITY);
  nvicEnableVector(AT32_EXINT1_NUMBER, AT32_IRQ_EXINT1_PRIORITY);
  nvicEnableVector(AT32_EXINT2_NUMBER, AT32_IRQ_EXINT2_PRIORITY);
  nvicEnableVector(AT32_EXINT3_NUMBER, AT32_IRQ_EXINT3_PRIORITY);
  nvicEnableVector(AT32_EXINT4_NUMBER, AT32_IRQ_EXINT4_PRIORITY);
  nvicEnableVector(AT32_EXINT5_9_NUMBER, AT32_IRQ_EXINT5_9_PRIORITY);
  nvicEnableVector(AT32_EXINT10_15_NUMBER, AT32_IRQ_EXINT10_15_PRIORITY);
  nvicEnableVector(AT32_EXINT16_NUMBER, AT32_IRQ_EXINT16_PRIORITY);
  nvicEnableVector(AT32_EXINT17_NUMBER, AT32_IRQ_EXINT17_PRIORITY);
  nvicEnableVector(AT32_EXINT18_NUMBER, AT32_IRQ_EXINT18_PRIORITY);
#endif

  tmr1_tmr9_tmr10_tmr11_irq_init();
  tmr2_irq_init();
  tmr3_irq_init();
  tmr4_irq_init();
  tmr5_irq_init();
  tmr6_irq_init();
  tmr7_irq_init();
  tmr8_tmr12_tmr13_tmr14_irq_init();

  usart1_irq_init();
  usart2_irq_init();
  usart3_irq_init();
  uart4_irq_init();
  uart5_irq_init();
  usart6_irq_init();
  uart7_irq_init();
  uart8_irq_init();
}

/**
 * @brief   Disables IRQ sources.
 *
 * @notapi
 */
void irqDeinit(void) {

#if HAL_USE_PAL
  nvicDisableVector(AT32_EXINT0_NUMBER);
  nvicDisableVector(AT32_EXINT1_NUMBER);
  nvicDisableVector(AT32_EXINT2_NUMBER);
  nvicDisableVector(AT32_EXINT3_NUMBER);
  nvicDisableVector(AT32_EXINT4_NUMBER);
  nvicDisableVector(AT32_EXINT5_9_NUMBER);
  nvicDisableVector(AT32_EXINT10_15_NUMBER);
  nvicDisableVector(AT32_EXINT16_NUMBER);
  nvicDisableVector(AT32_EXINT17_NUMBER);
  nvicDisableVector(AT32_EXINT18_NUMBER);
#endif

  tmr1_tmr9_tmr10_tmr11_irq_deinit();
  tmr2_irq_deinit();
  tmr3_irq_deinit();
  tmr4_irq_deinit();
  tmr5_irq_deinit();
  tmr6_irq_deinit();
  tmr7_irq_deinit();
  tmr8_tmr12_tmr13_tmr14_irq_deinit();

  usart1_irq_deinit();
  usart2_irq_deinit();
  usart3_irq_deinit();
  uart4_irq_deinit();
  uart5_irq_deinit();
  usart6_irq_deinit();
  uart7_irq_deinit();
  uart8_irq_deinit();
}

/** @} */
