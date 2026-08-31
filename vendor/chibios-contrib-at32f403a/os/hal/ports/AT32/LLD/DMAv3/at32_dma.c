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
 * @file    DMAv3/at32_dma.c
 * @brief   Enhanced DMA helper driver code.
 *
 * @addtogroup AT32_DMA
 * @details DMA sharing helper driver. In the AT32 the DMA streams are a
 *          shared resource, this driver allows to allocate and free DMA
 *          streams at runtime in order to allow all the other device
 *          drivers to coordinate the access to the resource.
 * @note    The DMA ISR handlers are all declared into this module because
 *          sharing, the various device drivers can associate a callback to
 *          ISRs when allocating streams.
 * @{
 */

#include "hal.h"

/* The following macro is only defined if some driver requiring DMA services
   has been enabled.*/
#if defined(AT32_DMA_REQUIRED) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/**
 * @brief   Mask of the DMA1 streams in @p dma.allocated_mask.
 */
#define AT32_DMA1_STREAMS_MASK     0x000000FFU

/**
 * @brief   Mask of the DMA2 streams in @p dma.allocated_mask.
 */
#define AT32_DMA2_STREAMS_MASK     0x0000FF00U

#if AT32_DMA_SUPPORTS_DMAMUX == TRUE

#define DMA1_CH1_VARIANT            DMA1MUX_CHANNEL1
#define DMA1_CH2_VARIANT            DMA1MUX_CHANNEL2
#define DMA1_CH3_VARIANT            DMA1MUX_CHANNEL3
#define DMA1_CH4_VARIANT            DMA1MUX_CHANNEL4
#define DMA1_CH5_VARIANT            DMA1MUX_CHANNEL5
#define DMA1_CH6_VARIANT            DMA1MUX_CHANNEL6
#define DMA1_CH7_VARIANT            DMA1MUX_CHANNEL7
#define DMA1_CH8_VARIANT            DMA1MUX_CHANNEL8

#if AT32_HAS_DMA2 == TRUE
#define DMA2_CH1_VARIANT            DMA2MUX_CHANNEL1
#define DMA2_CH2_VARIANT            DMA2MUX_CHANNEL2
#define DMA2_CH3_VARIANT            DMA2MUX_CHANNEL3
#define DMA2_CH4_VARIANT            DMA2MUX_CHANNEL4
#define DMA2_CH5_VARIANT            DMA2MUX_CHANNEL5
#define DMA2_CH6_VARIANT            DMA2MUX_CHANNEL6
#define DMA2_CH7_VARIANT            DMA2MUX_CHANNEL7
#define DMA2_CH8_VARIANT            DMA2MUX_CHANNEL8
#endif

#else /* !(AT32_DMA_SUPPORTS_DMAMUX == TRUE) */

#define DMA1_CH1_VARIANT            0
#define DMA1_CH2_VARIANT            0
#define DMA1_CH3_VARIANT            0
#define DMA1_CH4_VARIANT            0
#define DMA1_CH5_VARIANT            0
#define DMA1_CH6_VARIANT            0
#define DMA1_CH7_VARIANT            0
#define DMA1_CH8_VARIANT            0
#if AT32_HAS_DMA2 == TRUE
#define DMA2_CH1_VARIANT            0
#define DMA2_CH2_VARIANT            0
#define DMA2_CH3_VARIANT            0
#define DMA2_CH4_VARIANT            0
#define DMA2_CH5_VARIANT            0
#define DMA2_CH6_VARIANT            0
#define DMA2_CH7_VARIANT            0
#define DMA2_CH8_VARIANT            0
#endif

#endif /* !(AT32_DMA_SUPPORTS_DMAMUX == TRUE) */

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   DMA streams descriptors.
 * @details This table keeps the association between an unique stream
 *          identifier and the involved physical registers.
 * @note    Don't use this array directly, use the appropriate wrapper macros
 *          instead: @p AT32_DMA1_STREAM1, @p AT32_DMA1_STREAM2 etc.
 */
const at32_dma_stream_t _at32_dma_streams[AT32_DMA_STREAMS] = {
  {DMA1_STREAM1, &DMA1->CLR1, DMA1_CH1_VARIANT,  0,  0, AT32_DMA1_CH1_NUMBER},
  {DMA1_STREAM2, &DMA1->CLR1, DMA1_CH2_VARIANT,  6,  1, AT32_DMA1_CH2_NUMBER},
  {DMA1_STREAM3, &DMA1->CLR1, DMA1_CH3_VARIANT, 16,  2, AT32_DMA1_CH3_NUMBER},
  {DMA1_STREAM4, &DMA1->CLR1, DMA1_CH4_VARIANT, 22,  3, AT32_DMA1_CH4_NUMBER},
  {DMA1_STREAM5, &DMA1->CLR2, DMA1_CH5_VARIANT,  0,  4, AT32_DMA1_CH5_NUMBER},
  {DMA1_STREAM6, &DMA1->CLR2, DMA1_CH6_VARIANT,  6,  5, AT32_DMA1_CH6_NUMBER},
  {DMA1_STREAM7, &DMA1->CLR2, DMA1_CH7_VARIANT, 16,  6, AT32_DMA1_CH7_NUMBER},
  {DMA1_STREAM8, &DMA1->CLR2, DMA1_CH8_VARIANT, 22,  7, AT32_DMA1_CH8_NUMBER},
#if AT32_HAS_DMA2 == TRUE
  {DMA2_STREAM1, &DMA2->CLR1, DMA2_CH1_VARIANT,  0,  8, AT32_DMA2_CH1_NUMBER},
  {DMA2_STREAM2, &DMA2->CLR1, DMA2_CH2_VARIANT,  6,  9, AT32_DMA2_CH2_NUMBER},
  {DMA2_STREAM3, &DMA2->CLR1, DMA2_CH3_VARIANT, 16, 10, AT32_DMA2_CH3_NUMBER},
  {DMA2_STREAM4, &DMA2->CLR1, DMA2_CH4_VARIANT, 22, 11, AT32_DMA2_CH4_NUMBER},
  {DMA2_STREAM5, &DMA2->CLR2, DMA2_CH5_VARIANT,  0, 12, AT32_DMA2_CH5_NUMBER},
  {DMA2_STREAM6, &DMA2->CLR2, DMA2_CH6_VARIANT,  6, 13, AT32_DMA2_CH6_NUMBER},
  {DMA2_STREAM7, &DMA2->CLR2, DMA2_CH7_VARIANT, 16, 14, AT32_DMA2_CH7_NUMBER},
  {DMA2_STREAM8, &DMA2->CLR2, DMA2_CH8_VARIANT, 22, 15, AT32_DMA2_CH8_NUMBER},
#endif
};

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/**
 * @brief   Global DMA-related data structures.
 */
static struct {
  /**
   * @brief   Mask of the allocated streams.
   */
  uint32_t           allocated_mask;
  /**
   * @brief   DMA IRQ redirectors.
   */
  struct {
    /**
     * @brief   DMA callback function.
     */
    at32_dmasts_t    func;
    /**
     * @brief   DMA callback parameter.
     */
    void             *param;
  } streams[AT32_DMA_STREAMS];
} dma;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/**
 * @brief   DMA1 stream 1 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA1_CH1_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA1->STS1 >> 0U) & AT32_DMA_STS_MASK;
  DMA1->CLR1 = flags << 0U;
  if (dma.streams[0].func)
    dma.streams[0].func(dma.streams[0].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA1 stream 2 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA1_CH2_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA1->STS1 >> 6U) & AT32_DMA_STS_MASK;
  DMA1->CLR1 = flags << 6U;
  if (dma.streams[1].func)
    dma.streams[1].func(dma.streams[1].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA1 stream 3 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA1_CH3_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA1->STS1 >> 16U) & AT32_DMA_STS_MASK;
  DMA1->CLR1 = flags << 16U;
  if (dma.streams[2].func)
    dma.streams[2].func(dma.streams[2].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA1 stream 4 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA1_CH4_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA1->STS1 >> 22U) & AT32_DMA_STS_MASK;
  DMA1->CLR1 = flags << 22U;
  if (dma.streams[3].func)
    dma.streams[3].func(dma.streams[3].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA1 stream 5 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA1_CH5_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA1->STS2 >> 0U) & AT32_DMA_STS_MASK;
  DMA1->CLR2 = flags << 0U;
  if (dma.streams[4].func)
    dma.streams[4].func(dma.streams[4].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA1 stream 6 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA1_CH6_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA1->STS2 >> 6U) & AT32_DMA_STS_MASK;
  DMA1->CLR2 = flags << 6U;
  if (dma.streams[5].func)
    dma.streams[5].func(dma.streams[5].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA1 stream 7 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA1_CH7_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA1->STS2 >> 16U) & AT32_DMA_STS_MASK;
  DMA1->CLR2 = flags << 16U;
  if (dma.streams[6].func)
    dma.streams[6].func(dma.streams[6].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA1 stream 8 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA1_CH8_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA1->STS2 >> 22U) & AT32_DMA_STS_MASK;
  DMA1->CLR2 = flags << 22U;
  if (dma.streams[7].func)
    dma.streams[7].func(dma.streams[7].param, flags);

  OSAL_IRQ_EPILOGUE();
}

#if AT32_HAS_DMA2 == TRUE

/**
 * @brief   DMA2 stream 1 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA2_CH1_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA2->STS1 >> 0U) & AT32_DMA_STS_MASK;
  DMA2->CLR1 = flags << 0U;
  if (dma.streams[8].func)
    dma.streams[8].func(dma.streams[8].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA2 stream 2 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA2_CH2_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA2->STS1 >> 6U) & AT32_DMA_STS_MASK;
  DMA2->CLR1 = flags << 6U;
  if (dma.streams[9].func)
    dma.streams[9].func(dma.streams[9].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA2 stream 3 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA2_CH3_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA2->STS1 >> 16U) & AT32_DMA_STS_MASK;
  DMA2->CLR1 = flags << 16U;
  if (dma.streams[10].func)
    dma.streams[10].func(dma.streams[10].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA2 stream 4 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA2_CH4_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA2->STS1 >> 22U) & AT32_DMA_STS_MASK;
  DMA2->CLR1 = flags << 22U;
  if (dma.streams[11].func)
    dma.streams[11].func(dma.streams[11].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA2 stream 5 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA2_CH5_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA2->STS2 >> 0U) & AT32_DMA_STS_MASK;
  DMA2->CLR2 = flags << 0U;
  if (dma.streams[12].func)
    dma.streams[12].func(dma.streams[12].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA2 stream 6 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA2_CH6_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA2->STS2 >> 6U) & AT32_DMA_STS_MASK;
  DMA2->CLR2 = flags << 6U;
  if (dma.streams[13].func)
    dma.streams[13].func(dma.streams[13].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA2 stream 7 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA2_CH7_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA2->STS2 >> 16U) & AT32_DMA_STS_MASK;
  DMA2->CLR2 = flags << 16U;
  if (dma.streams[14].func)
    dma.streams[14].func(dma.streams[14].param, flags);

  OSAL_IRQ_EPILOGUE();
}

/**
 * @brief   DMA2 stream 8 shared interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(AT32_DMA2_CH8_HANDLER) {
  uint32_t flags;

  OSAL_IRQ_PROLOGUE();

  flags = (DMA2->STS2 >> 22U) & AT32_DMA_STS_MASK;
  DMA2->CLR2 = flags << 22U;
  if (dma.streams[15].func)
    dma.streams[15].func(dma.streams[15].param, flags);

  OSAL_IRQ_EPILOGUE();
}

#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   AT32 DMA helper initialization.
 *
 * @init
 */
void dmaInit(void) {
  unsigned i;

  dma.allocated_mask = 0U;
  for (i = 0U; i < AT32_DMA_STREAMS; i++) {
    _at32_dma_streams[i].stream->CTRL = AT32_DMA_SCTRL_RESET_VALUE;
    dma.streams[i].func = NULL;
  }
  DMA1->CLR1 = 0xFFFFFFFFU;
  DMA1->CLR2 = 0xFFFFFFFFU;
#if AT32_HAS_DMA2 == TRUE
  DMA2->CLR1 = 0xFFFFFFFFU;
  DMA2->CLR2 = 0xFFFFFFFFU;
#endif
}

/**
 * @brief   Allocates a DMA stream.
 * @details The stream is allocated and, if required, the DMA clock enabled.
 *          The function also enables the IRQ vector associated to the stream
 *          and initializes its priority.
 *
 * @param[in] id        numeric identifiers of a specific stream or:
 *                      - @p AT32_DMA_STREAM_ID_ANY for any stream.
 *                      - @p AT32_DMA_STREAM_ID_ANY_DMA1 for any stream
 *                        on DMA1.
 *                      - @p AT32_DMA_STREAM_ID_ANY_DMA2 for any stream
 *                        on DMA2.
 *                      .
 * @param[in] priority  IRQ priority for the DMA stream
 * @param[in] func      handling function pointer, can be @p NULL
 * @param[in] param     a parameter to be passed to the handling function
 * @return              Pointer to the allocated @p at32_dma_stream_t
 *                      structure.
 * @retval NULL         if a/the stream is not available.
 *
 * @iclass
 */
const at32_dma_stream_t *dmaStreamAllocI(uint32_t id,
                                          uint32_t priority,
                                          at32_dmasts_t func,
                                          void *param) {
  uint32_t i, startid, endid;

  osalDbgCheckClassI();

  if (id < AT32_DMA_STREAMS) {
    startid = id;
    endid   = id;
  }
#if AT32_DMA_SUPPORTS_DMAMUX == TRUE
  else if (id == AT32_DMA_STREAM_ID_ANY) {
    startid = 0U;
    endid   = AT32_DMA_STREAMS - 1U;
  }
  else if (id == AT32_DMA_STREAM_ID_ANY_DMA1) {
    startid = 0U;
    endid   = (AT32_DMA_STREAMS / 2U) - 1U;
  }
#if AT32_HAS_DMA2 == TRUE
  else if (id == AT32_DMA_STREAM_ID_ANY_DMA2) {
    startid = (AT32_DMA_STREAMS / 2U) - 1U;
    endid   = AT32_DMA_STREAMS - 1U;
  }
#endif
#endif
  else {
    osalDbgCheck(false);
    return NULL;
  }

  for (i = startid; i <= endid; i++) {
    uint32_t mask = (1U << i);
    if ((dma.allocated_mask & mask) == 0U) {
      const at32_dma_stream_t *dmastp = AT32_DMA_STREAM(i);

      /* Installs the DMA handler.*/
      dma.streams[i].func  = func;
      dma.streams[i].param = param;
      dma.allocated_mask  |= mask;

      /* Enabling DMA clocks required by the current streams set.*/
      if ((AT32_DMA1_STREAMS_MASK & mask) != 0U) {
        crmEnableDMA1(true);
      }
#if AT32_HAS_DMA2 == TRUE
      if ((AT32_DMA2_STREAMS_MASK & mask) != 0U) {
        crmEnableDMA2(true);
      }
#endif

#if (AT32_DMA_SUPPORTS_DMAMUX == TRUE) && defined(crmEnableDMAMUX)
      /* Enabling DMAMUX if present.*/
      if (dma.allocated_mask != 0U) {
        crmEnableDMAMUX(true);
      }
#endif
#if AT32_DMA_SUPPORTS_DMAMUX == TRUE
      DMA1->MUXSEL = DMA_MUXSEL_TBL_SEL;
#if AT32_HAS_DMA2 == TRUE
      DMA2->MUXSEL = DMA_MUXSEL_TBL_SEL;
#endif
#endif
      /* Putting the stream in a safe state.*/
      dmaStreamDisable(dmastp);
      dmastp->stream->CTRL = AT32_DMA_SCTRL_RESET_VALUE;
      dmastp->stream->FCTRL = AT32_DMA_FCTRL_RESET_VALUE;

      /* Enables the associated IRQ vector if a callback is defined.*/
      if (func != NULL) {
        nvicEnableVector(dmastp->vector, priority);
      }

      return dmastp;
    }
  }

  return NULL;
}

/**
 * @brief   Allocates a DMA stream.
 * @details The stream is allocated and, if required, the DMA clock enabled.
 *          The function also enables the IRQ vector associated to the stream
 *          and initializes its priority.
 *
 * @param[in] id        numeric identifiers of a specific stream or:
 *                      - @p AT32_DMA_STREAM_ID_ANY for any stream.
 *                      - @p AT32_DMA_STREAM_ID_ANY_DMA1 for any stream
 *                        on DMA1.
 *                      - @p AT32_DMA_STREAM_ID_ANY_DMA2 for any stream
 *                        on DMA2.
 *                      .
 * @param[in] priority  IRQ priority for the DMA stream
 * @param[in] func      handling function pointer, can be @p NULL
 * @param[in] param     a parameter to be passed to the handling function
 * @return              Pointer to the allocated @p at32_dma_stream_t
 *                      structure.
 * @retval NULL         if a/the stream is not available.
 *
 * @api
 */
const at32_dma_stream_t *dmaStreamAlloc(uint32_t id,
                                         uint32_t priority,
                                         at32_dmasts_t func,
                                         void *param) {
  const at32_dma_stream_t *dmastp;

  osalSysLock();
  dmastp = dmaStreamAllocI(id, priority, func, param);
  osalSysUnlock();

  return dmastp;
}

/**
 * @brief   Releases a DMA stream.
 * @details The stream is freed and, if required, the DMA clock disabled.
 *          Trying to release a unallocated stream is an illegal operation
 *          and is trapped if assertions are enabled.
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 *
 * @iclass
 */
void dmaStreamFreeI(const at32_dma_stream_t *dmastp) {

  osalDbgCheck(dmastp != NULL);

  /* Check if the streams is not taken.*/
  osalDbgAssert((dma.allocated_mask & (1U << dmastp->selfindex)) != 0U,
                "not allocated");

  /* Disables the associated IRQ vector.*/
  nvicDisableVector(dmastp->vector);

  /* Marks the stream as not allocated.*/
  dma.allocated_mask &= ~(1U << dmastp->selfindex);

  /* Shutting down clocks that are no more required, if any.*/
  if ((dma.allocated_mask & AT32_DMA1_STREAMS_MASK) == 0U) {
    crmDisableDMA1();
  }
#if AT32_HAS_DMA2 == TRUE
  if ((dma.allocated_mask & AT32_DMA2_STREAMS_MASK) == 0U) {
    crmDisableDMA2();
  }
#endif

#if (AT32_DMA_SUPPORTS_DMAMUX == TRUE) && defined(crmDisableDMAMUX)
  /* Shutting down DMAMUX if present.*/
  if (dma.allocated_mask == 0U) {
    crmDisableDMAMUX();
  }
#endif
}

/**
 * @brief   Releases a DMA stream.
 * @details The stream is freed and, if required, the DMA clock disabled.
 *          Trying to release a unallocated stream is an illegal operation
 *          and is trapped if assertions are enabled.
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 *
 * @api
 */
void dmaStreamFree(const at32_dma_stream_t *dmastp) {

  osalSysLock();
  dmaStreamFreeI(dmastp);
  osalSysUnlock();
}

#if (AT32_DMA_SUPPORTS_DMAMUX == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Associates a peripheral request to a DMA stream.
 * @note    This function can be invoked in both ISR or thread context.
 *
 * @param[in] dmastp    pointer to a @p at32_dma_stream_t structure
 * @param[in] per       peripheral identifier
 *
 * @special
 */
void dmaSetRequestSource(const at32_dma_stream_t *dmastp, uint32_t per) {

  osalDbgCheck(per < 128U);

  dmastp->mux->MUXCTRL = per;
}
#endif

#endif /* AT32_DMA_REQUIRED */

/** @} */
