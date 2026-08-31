/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

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
 * @file    EDMAv1/at32_edma.c
 * @brief   Enhanced EDMA helper driver code.
 *
 * @addtogroup AT32_EDMA
 * @details EDMA sharing helper driver. In the AT32 the EDMA streams are a
 *          shared resource, this driver allows to allocate and free EDMA
 *          streams at runtime in order to allow all the other device
 *          drivers to coordinate the access to the resource.
 * @note    The EDMA ISR handlers are all declared into this module because
 *          sharing, the various device drivers can associate a callback to
 *          ISRs when allocating streams.
 * @{
 */

#include "hal.h"

/* The following macro is only defined if some driver requiring EDMA services
   has been enabled.*/
#if defined(AT32_EDMA_REQUIRED) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/**
 * @brief   Mask of the EDMA1 streams in @p edma.allocated_mask.
 */
#define AT32_EDMA1_STREAMS_MASK     0x000000FFU

/**
 * @brief   Mask of the EDMA2 streams in @p edma.allocated_mask.
 */
#define AT32_EDMA2_STREAMS_MASK     0x0000FF00U

#if AT32_EDMA_SUPPORTS_EDMAMUX == TRUE

#define EDMA1_CH1_VARIANT            EDMA1MUX_CHANNEL1
#define EDMA1_CH2_VARIANT            EDMA1MUX_CHANNEL2
#define EDMA1_CH3_VARIANT            EDMA1MUX_CHANNEL3
#define EDMA1_CH4_VARIANT            EDMA1MUX_CHANNEL4
#define EDMA1_CH5_VARIANT            EDMA1MUX_CHANNEL5
#define EDMA1_CH6_VARIANT            EDMA1MUX_CHANNEL6
#define EDMA1_CH7_VARIANT            EDMA1MUX_CHANNEL7
#define EDMA1_CH8_VARIANT            EDMA1MUX_CHANNEL8

#if AT32_HAS_EDMA2 == TRUE
#define EDMA2_CH1_VARIANT            EDMA2MUX_CHANNEL1
#define EDMA2_CH2_VARIANT            EDMA2MUX_CHANNEL2
#define EDMA2_CH3_VARIANT            EDMA2MUX_CHANNEL3
#define EDMA2_CH4_VARIANT            EDMA2MUX_CHANNEL4
#define EDMA2_CH5_VARIANT            EDMA2MUX_CHANNEL5
#define EDMA2_CH6_VARIANT            EDMA2MUX_CHANNEL6
#define EDMA2_CH7_VARIANT            EDMA2MUX_CHANNEL7
#define EDMA2_CH8_VARIANT            EDMA2MUX_CHANNEL8
#endif

#else /* !(AT32_EDMA_SUPPORTS_EDMAMUX == TRUE) */

#define EDMA1_CH1_VARIANT            0
#define EDMA1_CH2_VARIANT            0
#define EDMA1_CH3_VARIANT            0
#define EDMA1_CH4_VARIANT            0
#define EDMA1_CH5_VARIANT            0
#define EDMA1_CH6_VARIANT            0
#define EDMA1_CH7_VARIANT            0
#define EDMA1_CH8_VARIANT            0
#if AT32_HAS_EDMA2 == TRUE
#define EDMA2_CH1_VARIANT            0
#define EDMA2_CH2_VARIANT            0
#define EDMA2_CH3_VARIANT            0
#define EDMA2_CH4_VARIANT            0
#define EDMA2_CH5_VARIANT            0
#define EDMA2_CH6_VARIANT            0
#define EDMA2_CH7_VARIANT            0
#define EDMA2_CH8_VARIANT            0
#endif

#endif /* !(AT32_EDMA_SUPPORTS_EDMAMUX == TRUE) */

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   EDMA streams descriptors.
 * @details This table keeps the association between an unique stream
 *          identifier and the involved physical registers.
 * @note    Don't use this array directly, use the appropriate wrapper macros
 *          instead: @p AT32_EDMA1_STREAM1, @p AT32_EDMA1_STREAM2 etc.
 */
const at32_edma_stream_t _at32_edma_streams[AT32_EDMA_STREAMS] = {
  {EDMA1_STREAM1, &EDMA1->CLR1, EDMA1_CH1_VARIANT,  0,  0, AT32_EDMA1_CH1_NUMBER},
  {EDMA1_STREAM2, &EDMA1->CLR1, EDMA1_CH2_VARIANT,  6,  1, AT32_EDMA1_CH2_NUMBER},
  {EDMA1_STREAM3, &EDMA1->CLR1, EDMA1_CH3_VARIANT, 16,  2, AT32_EDMA1_CH3_NUMBER},
  {EDMA1_STREAM4, &EDMA1->CLR1, EDMA1_CH4_VARIANT, 22,  3, AT32_EDMA1_CH4_NUMBER},
  {EDMA1_STREAM5, &EDMA1->CLR2, EDMA1_CH5_VARIANT,  0,  4, AT32_EDMA1_CH5_NUMBER},
  {EDMA1_STREAM6, &EDMA1->CLR2, EDMA1_CH6_VARIANT,  6,  5, AT32_EDMA1_CH6_NUMBER},
  {EDMA1_STREAM7, &EDMA1->CLR2, EDMA1_CH7_VARIANT, 16,  6, AT32_EDMA1_CH7_NUMBER},
  {EDMA1_STREAM8, &EDMA1->CLR2, EDMA1_CH8_VARIANT, 22,  7, AT32_EDMA1_CH8_NUMBER},
#if AT32_HAS_EDMA2 == TRUE
  {EDMA2_STREAM1, &EDMA2->CLR1, EDMA2_CH1_VARIANT,  0,  8, AT32_EDMA2_CH1_NUMBER},
  {EDMA2_STREAM2, &EDMA2->CLR1, EDMA2_CH2_VARIANT,  6,  9, AT32_EDMA2_CH2_NUMBER},
  {EDMA2_STREAM3, &EDMA2->CLR1, EDMA2_CH3_VARIANT, 16, 10, AT32_EDMA2_CH3_NUMBER},
  {EDMA2_STREAM4, &EDMA2->CLR1, EDMA2_CH4_VARIANT, 22, 11, AT32_EDMA2_CH4_NUMBER},
  {EDMA2_STREAM5, &EDMA2->CLR2, EDMA2_CH5_VARIANT,  0, 12, AT32_EDMA2_CH5_NUMBER},
  {EDMA2_STREAM6, &EDMA2->CLR2, EDMA2_CH6_VARIANT,  6, 13, AT32_EDMA2_CH6_NUMBER},
  {EDMA2_STREAM7, &EDMA2->CLR2, EDMA2_CH7_VARIANT, 16, 14, AT32_EDMA2_CH7_NUMBER},
  {EDMA2_STREAM8, &EDMA2->CLR2, EDMA2_CH8_VARIANT, 22, 15, AT32_EDMA2_CH8_NUMBER},
#endif
};

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/**
 * @brief   Global EDMA-related data structures.
 */
static struct {
  /**
   * @brief   Mask of the allocated streams.
   */
  uint32_t           allocated_mask;
  /**
   * @brief   EDMA IRQ redirectors.
   */
  struct {
    /**
     * @brief   EDMA callback function.
     */
    at32_edmasts_t    func;
    /**
     * @brief   EDMA callback parameter.
     */
    void             *param;
  } streams[AT32_EDMA_STREAMS];
} edma;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/**
 * @brief   EDMA1 stream 1 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA1_CH1_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA1->STS1 >> 0U) & AT32_EDMA_STS_MASK;
  EDMA1->CLR1 = flags << 0U;
  if (edma.streams[0].func)
    edma.streams[0].func(edma.streams[0].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA1 stream 2 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA1_CH2_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA1->STS1 >> 6U) & AT32_EDMA_STS_MASK;
  EDMA1->CLR1 = flags << 6U;
  if (edma.streams[1].func)
    edma.streams[1].func(edma.streams[1].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA1 stream 3 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA1_CH3_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA1->STS1 >> 16U) & AT32_EDMA_STS_MASK;
  EDMA1->CLR1 = flags << 16U;
  if (edma.streams[2].func)
    edma.streams[2].func(edma.streams[2].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA1 stream 4 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA1_CH4_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA1->STS1 >> 22U) & AT32_EDMA_STS_MASK;
  EDMA1->CLR1 = flags << 22U;
  if (edma.streams[3].func)
    edma.streams[3].func(edma.streams[3].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA1 stream 5 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA1_CH5_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA1->STS2 >> 0U) & AT32_EDMA_STS_MASK;
  EDMA1->CLR2 = flags << 0U;
  if (edma.streams[4].func)
    edma.streams[4].func(edma.streams[4].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA1 stream 6 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA1_CH6_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA1->STS2 >> 6U) & AT32_EDMA_STS_MASK;
  EDMA1->CLR2 = flags << 6U;
  if (edma.streams[5].func)
    edma.streams[5].func(edma.streams[5].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA1 stream 7 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA1_CH7_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA1->STS2 >> 16U) & AT32_EDMA_STS_MASK;
  EDMA1->CLR2 = flags << 16U;
  if (edma.streams[6].func)
    edma.streams[6].func(edma.streams[6].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA1 stream 8 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA1_CH8_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA1->STS2 >> 22U) & AT32_EDMA_STS_MASK;
  EDMA1->CLR2 = flags << 22U;
  if (edma.streams[7].func)
    edma.streams[7].func(edma.streams[7].param, flags);

  OSAL_IRQ_EPILOGUE();
}

#if AT32_HAS_EDMA2 == TRUE

/**
 * @brief   EDMA2 stream 1 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA2_CH1_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA2->STS1 >> 0U) & AT32_EDMA_STS_MASK;
  EDMA2->CLR1 = flags << 0U;
  if (edma.streams[8].func)
    edma.streams[8].func(edma.streams[8].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA2 stream 2 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA2_CH2_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA2->STS1 >> 6U) & AT32_EDMA_STS_MASK;
  EDMA2->CLR1 = flags << 6U;
  if (edma.streams[9].func)
    edma.streams[9].func(edma.streams[9].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA2 stream 3 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA2_CH3_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA2->STS1 >> 16U) & AT32_EDMA_STS_MASK;
  EDMA2->CLR1 = flags << 16U;
  if (edma.streams[10].func)
    edma.streams[10].func(edma.streams[10].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA2 stream 4 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA2_CH4_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA2->STS1 >> 22U) & AT32_EDMA_STS_MASK;
  EDMA2->CLR1 = flags << 22U;
  if (edma.streams[11].func)
    edma.streams[11].func(edma.streams[11].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA2 stream 5 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA2_CH5_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA2->STS2 >> 0U) & AT32_EDMA_STS_MASK;
  EDMA2->CLR2 = flags << 0U;
  if (edma.streams[12].func)
    edma.streams[12].func(edma.streams[12].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA2 stream 6 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA2_CH6_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA2->STS2 >> 6U) & AT32_EDMA_STS_MASK;
  EDMA2->CLR2 = flags << 6U;
  if (edma.streams[13].func)
    edma.streams[13].func(edma.streams[13].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA2 stream 7 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA2_CH7_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA2->STS2 >> 16U) & AT32_EDMA_STS_MASK;
  EDMA2->CLR2 = flags << 16U;
  if (edma.streams[14].func)
    edma.streams[14].func(edma.streams[14].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   EDMA2 stream 8 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_EDMA2_CH8_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (EDMA2->STS2 >> 22U) & AT32_EDMA_STS_MASK;
  EDMA2->CLR2 = flags << 22U;
  if (edma.streams[15].func)
    edma.streams[15].func(edma.streams[15].param, flags);

  OSAL_IRQ_EPILOGUE();
}

#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   AT32 EDMA helper initialization.
 *
 * @init
 */
void edmaInit(void) {
  unsigned i;

  edma.allocated_mask = 0U;
  for (i = 0U; i < AT32_EDMA_STREAMS; i++) {
    _at32_edma_streams[i].stream->CTRL = AT32_EDMA_SCTRL_RESET_VALUE;
    edma.streams[i].func = NULL;
  }
  EDMA1->CLR1 = 0xFFFFFFFFU;
  EDMA1->CLR2 = 0xFFFFFFFFU;
#if AT32_HAS_EDMA2 == TRUE
  EDMA2->CLR1 = 0xFFFFFFFFU;
  EDMA2->CLR2 = 0xFFFFFFFFU;
#endif
}

/**
 * @brief   Allocates a EDMA stream.
 * @details The stream is allocated and, if required, the EDMA clock enabled.
 *          The function also enables the IRQ vector associated to the stream
 *          and initializes its priority.
 *
 * @param[in] id        numeric identifiers of a specific stream or:
 *                      - @p AT32_EDMA_STREAM_ID_ANY for any stream.
 *                      - @p AT32_EDMA_STREAM_ID_ANY_EDMA1 for any stream
 *                        on EDMA1.
 *                      - @p AT32_EDMA_STREAM_ID_ANY_EDMA2 for any stream
 *                        on EDMA2.
 *                      .
 * @param[in] priority  IRQ priority for the EDMA stream
 * @param[in] func      handling function pointer, can be @p NULL
 * @param[in] param     a parameter to be passed to the handling function
 * @return              Pointer to the allocated @p at32_edma_stream_t
 *                      structure.
 * @retval NULL         if a/the stream is not available.
 *
 * @iclass
 */
const at32_edma_stream_t *edmaStreamAllocI(uint32_t id,
                                          uint32_t priority,
                                          at32_edmasts_t func,
                                          void *param) {
  uint32_t i, startid, endid;

  osalDbgCheckClassI();

  if (id < AT32_EDMA_STREAMS) {
    startid = id;
    endid   = id;
  }
#if AT32_EDMA_SUPPORTS_EDMAMUX == TRUE
  else if (id == AT32_EDMA_STREAM_ID_ANY) {
    startid = 0U;
    endid   = AT32_EDMA_STREAMS - 1U;
  }
  else if (id == AT32_EDMA_STREAM_ID_ANY_EDMA1) {
    startid = 0U;
    endid   = (AT32_EDMA_STREAMS / 2U) - 1U;
  }
#if AT32_HAS_EDMA2 == TRUE
  else if (id == AT32_EDMA_STREAM_ID_ANY_EDMA2) {
    startid = (AT32_EDMA_STREAMS / 2U) - 1U;
    endid   = AT32_EDMA_STREAMS - 1U;
  }
#endif
#endif
  else {
    osalDbgCheck(false);
    return NULL;
  }

  for (i = startid; i <= endid; i++) {
    uint32_t mask = (1U << i);
    if ((edma.allocated_mask & mask) == 0U) {
      const at32_edma_stream_t *edmastp = AT32_EDMA_STREAM(i);

      /* Installs the EDMA handler.*/
      edma.streams[i].func  = func;
      edma.streams[i].param = param;
      edma.allocated_mask  |= mask;

      /* Enabling EDMA clocks required by the current streams set.*/
      if ((AT32_EDMA1_STREAMS_MASK & mask) != 0U) {
        crmEnableEDMA1(true);
      }
#if AT32_HAS_EDMA2 == TRUE
      if ((AT32_EDMA2_STREAMS_MASK & mask) != 0U) {
        crmEnableEDMA2(true);
      }
#endif

#if (AT32_EDMA_SUPPORTS_EDMAMUX == TRUE) && defined(crmEnableEDMAMUX)
      /* Enabling EDMAMUX if present.*/
      if (edma.allocated_mask != 0U) {
        crmEnableEDMAMUX(true);
      }
#endif
#if AT32_EDMA_SUPPORTS_EDMAMUX == TRUE
      EDMA1->MUXSEL = EDMA_MUXSEL_TBL_SEL;
#if AT32_HAS_EDMA2 == TRUE
      EDMA2->MUXSEL = EDMA_MUXSEL_TBL_SEL;
#endif
#endif
      /* Putting the stream in a safe state.*/
      edmaStreamDisable(edmastp);
      edmastp->stream->CTRL = AT32_EDMA_SCTRL_RESET_VALUE;
      edmastp->stream->FCTRL = AT32_EDMA_FCTRL_RESET_VALUE;

      /* Enables the associated IRQ vector if a callback is defined.*/
      if (func != NULL) {
        nvicEnableVector(edmastp->vector, priority);
      }

      return edmastp;
    }
  }

  return NULL;
}

/**
 * @brief   Allocates a EDMA stream.
 * @details The stream is allocated and, if required, the EDMA clock enabled.
 *          The function also enables the IRQ vector associated to the stream
 *          and initializes its priority.
 *
 * @param[in] id        numeric identifiers of a specific stream or:
 *                      - @p AT32_EDMA_STREAM_ID_ANY for any stream.
 *                      - @p AT32_EDMA_STREAM_ID_ANY_EDMA1 for any stream
 *                        on EDMA1.
 *                      - @p AT32_EDMA_STREAM_ID_ANY_EDMA2 for any stream
 *                        on EDMA2.
 *                      .
 * @param[in] priority  IRQ priority for the EDMA stream
 * @param[in] func      handling function pointer, can be @p NULL
 * @param[in] param     a parameter to be passed to the handling function
 * @return              Pointer to the allocated @p at32_edma_stream_t
 *                      structure.
 * @retval NULL         if a/the stream is not available.
 *
 * @api
 */
const at32_edma_stream_t *edmaStreamAlloc(uint32_t id,
                                         uint32_t priority,
                                         at32_edmasts_t func,
                                         void *param) {
  const at32_edma_stream_t *edmastp;

  osalSysLock();
  edmastp = edmaStreamAllocI(id, priority, func, param);
  osalSysUnlock();

  return edmastp;
}

/**
 * @brief   Releases a EDMA stream.
 * @details The stream is freed and, if required, the EDMA clock disabled.
 *          Trying to release a unallocated stream is an illegal operation
 *          and is trapped if assertions are enabled.
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 *
 * @iclass
 */
void edmaStreamFreeI(const at32_edma_stream_t *edmastp) {

  osalDbgCheck(edmastp != NULL);

  /* Check if the streams is not taken.*/
  osalDbgAssert((edma.allocated_mask & (1U << edmastp->selfindex)) != 0U,
                "not allocated");

  /* Disables the associated IRQ vector.*/
  nvicDisableVector(edmastp->vector);

  /* Marks the stream as not allocated.*/
  edma.allocated_mask &= ~(1U << edmastp->selfindex);

  /* Shutting down clocks that are no more required, if any.*/
  if ((edma.allocated_mask & AT32_EDMA1_STREAMS_MASK) == 0U) {
    crmDisableEDMA1();
  }
#if AT32_HAS_EDMA2 == TRUE
  if ((edma.allocated_mask & AT32_EDMA2_STREAMS_MASK) == 0U) {
    crmDisableEDMA2();
  }
#endif

#if (AT32_EDMA_SUPPORTS_EDMAMUX == TRUE) && defined(crmDisableEDMAMUX)
  /* Shutting down EDMAMUX if present.*/
  if (edma.allocated_mask == 0U) {
    crmDisableEDMAMUX();
  }
#endif
}

/**
 * @brief   Releases a EDMA stream.
 * @details The stream is freed and, if required, the EDMA clock disabled.
 *          Trying to release a unallocated stream is an illegal operation
 *          and is trapped if assertions are enabled.
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 *
 * @api
 */
void edmaStreamFree(const at32_edma_stream_t *edmastp) {

  osalSysLock();
  edmaStreamFreeI(edmastp);
  osalSysUnlock();
}

#if (AT32_EDMA_SUPPORTS_EDMAMUX == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Associates a peripheral request to a EDMA stream.
 * @note    This function can be invoked in both ISR or thread context.
 *
 * @param[in] edmastp    pointer to a @p at32_edma_stream_t structure
 * @param[in] per       peripheral identifier
 *
 * @special
 */
void edmaSetRequestSource(const at32_edma_stream_t *edmastp, uint32_t per) {

  osalDbgCheck(per < 128U);

  edmastp->mux->MUXCTRL = per;
}
#endif

#endif /* AT32_EDMA_REQUIRED */

/** @} */
