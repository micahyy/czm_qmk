/*
    ChibiOS - Copyright (C) 2006..2019 Giovanni Di Sirio

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
 * @file    DMAv3/at32_dma.h
 * @brief   Enhanced-DMA helper driver header.
 *
 * @addtogroup AT32_DMA
 * @{
 */

#ifndef AT32_DMA_H
#define AT32_DMA_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief   Requires use of DMAv3 driver model.
 */
#define AT32_USE_DMA_V3            TRUE

/**
 * @brief   DMA capability.
 * @details if @p TRUE then the DMA is able of burst transfers, FIFOs,
 *          scatter gather and other advanced features.
 */
#define AT32_DMA_ADVANCED          TRUE

/**
 * @brief   Total number of DMA streams.
 * @details This is the total number of streams among all the DMA units.
 */
#if AT32_HAS_DMA2 == TRUE
#define AT32_DMA_STREAMS           16U
#else
#define AT32_DMA_STREAMS           8U
#endif

/**
 * @brief   Mask of the STS bits passed to the DMA callback functions.
 */
#define AT32_DMA_STS_MASK          0x3DU

/**
 * @brief   Returns the channel associated to the specified stream.
 *
 * @param[in] id        the unique numeric stream identifier
 * @param[in] c         a stream/channel association word, one channel per
 *                      nibble
 * @return              Returns the channel associated to the stream.
 */
#define AT32_DMA_GETCHANNEL(id, c) (((c) >> (((id) & 7U) * 4U)) & 15U)

/**
 * @brief   Checks if a DMA priority is within the valid range.
 * @param[in] prio      DMA priority
 *
 * @retval              The check result.
 * @retval false        invalid DMA priority.
 * @retval true         correct DMA priority.
 */
#define AT32_DMA_IS_VALID_PRIORITY(prio) (((prio) >= 0U) && ((prio) <= 3U))

#if (AT32_DMA_SUPPORTS_DMAMUX == FALSE) || defined(_DOXYGEN__)
/**
 * @brief   Checks if a DMA stream id is within the valid range.
 *
 * @param[in] id        DMA stream id
 * @retval              The check result.
 * @retval false        invalid DMA stream.
 * @retval true         correct DMA stream.
 */
#define AT32_DMA_IS_VALID_STREAM(id) (((id) >= 0U) &&                      \
                                       ((id) < AT32_DMA_STREAMS))
#else /* AT32_DMA_SUPPORTS_DMAMUX == FALSE */
#if AT32_HAS_DMA2 == TRUE
#define AT32_DMA_IS_VALID_STREAM(id) (((id) >= 0U) &&                      \
                                       ((id) <= (AT32_DMA_STREAMS + 2)))
#else
#define AT32_DMA_IS_VALID_STREAM(id) (((id) >= 0U) &&                      \
                                       ((id) <= (AT32_DMA_STREAMS + 1)))
#endif
#endif /* AT32_DMA_SUPPORTS_DMAMUX == FALSE */

/**
 * @brief   Returns an unique numeric identifier for a DMA stream.
 *
 * @param[in] dma       the DMA unit number
 * @param[in] stream    the stream number
 * @return              An unique numeric stream identifier.
 */
#define AT32_DMA_STREAM_ID(dma, stream) ((((dma) - 1U) * 8U) + ((stream) - 1))

/**
 * @brief   Returns a DMA stream identifier mask.
 *
 *
 * @param[in] dma       the DMA unit number
 * @param[in] stream    the stream number
 * @return              A DMA stream identifier mask.
 */
#define AT32_DMA_STREAM_ID_MSK(dma, stream)                                \
  (1U << AT32_DMA_STREAM_ID(dma, stream))

/**
 * @brief   Checks if a DMA stream unique identifier belongs to a mask.
 * @param[in] id        the stream numeric identifier
 * @param[in] mask      the stream numeric identifiers mask
 *
 * @retval              The check result.
 * @retval false        id does not belong to the mask.
 * @retval true         id belongs to the mask.
 */
#define AT32_DMA_IS_VALID_ID(id, mask) (((1U << (id)) & (mask)))

#if (AT32_DMA_SUPPORTS_DMAMUX == TRUE) || defined(_DOXYGEN__)
/**
 * @name    Special stream identifiers
 * @{
 */
#define AT32_DMA_STREAM_ID_ANY         AT32_DMA_STREAMS
#define AT32_DMA_STREAM_ID_ANY_DMA1    (AT32_DMA_STREAM_ID_ANY + 1)
#if AT32_HAS_DMA2 == TRUE
#define AT32_DMA_STREAM_ID_ANY_DMA2    (AT32_DMA_STREAM_ID_ANY_DMA1 + 1)
#endif
/** @} */
#endif

/**
 * @name    DMA streams identifiers
 * @{
 */
/**
 * @brief   Returns a pointer to a at32_dma_stream_t structure.
 *
 * @param[in] id        the stream numeric identifier
 * @return              A pointer to the at32_dma_stream_t constant structure
 *                      associated to the DMA stream.
 */
#define AT32_DMA_STREAM(id)        (&_at32_dma_streams[id])

#define AT32_DMA1_STREAM1          AT32_DMA_STREAM(0)
#define AT32_DMA1_STREAM2          AT32_DMA_STREAM(1)
#define AT32_DMA1_STREAM3          AT32_DMA_STREAM(2)
#define AT32_DMA1_STREAM4          AT32_DMA_STREAM(3)
#define AT32_DMA1_STREAM5          AT32_DMA_STREAM(4)
#define AT32_DMA1_STREAM6          AT32_DMA_STREAM(5)
#define AT32_DMA1_STREAM7          AT32_DMA_STREAM(6)
#define AT32_DMA1_STREAM8          AT32_DMA_STREAM(7)
#if AT32_HAS_DMA2 == TRUE
#define AT32_DMA2_STREAM1          AT32_DMA_STREAM(8)
#define AT32_DMA2_STREAM2          AT32_DMA_STREAM(9)
#define AT32_DMA2_STREAM3          AT32_DMA_STREAM(10)
#define AT32_DMA2_STREAM4          AT32_DMA_STREAM(11)
#define AT32_DMA2_STREAM5          AT32_DMA_STREAM(12)
#define AT32_DMA2_STREAM6          AT32_DMA_STREAM(13)
#define AT32_DMA2_STREAM7          AT32_DMA_STREAM(14)
#define AT32_DMA2_STREAM8          AT32_DMA_STREAM(15)
#endif
/** @} */

/**
 * @name    CTRL register constants common to all DMA types
 * @{
 */
#define AT32_DMA_SCTRL_RESET_VALUE    0x00000000U
#define AT32_DMA_CTRL_SEN             DMA_SCTRL_SEN
#define AT32_DMA_CTRL_DTERRIEN        DMA_SCTRL_DTERRIEN
#define AT32_DMA_CTRL_HDTIEN          DMA_SCTRL_HDTIEN
#define AT32_DMA_CTRL_FDTIEN          DMA_SCTRL_FDTIEN
#define AT32_DMA_CTRL_DTD_MASK        DMA_SCTRL_DTD
#define AT32_DMA_CTRL_DTD_P2M         0U
#define AT32_DMA_CTRL_DTD_M2P         DMA_SCTRL_DTD_M2P
#define AT32_DMA_CTRL_DTD_M2M         DMA_SCTRL_DTD_M2M
#define AT32_DMA_CTRL_LM              DMA_SCTRL_LM
#define AT32_DMA_CTRL_PINCM           DMA_SCTRL_PINCM
#define AT32_DMA_CTRL_MINCM           DMA_SCTRL_MINCM
#define AT32_DMA_CTRL_PWIDTH_MASK     DMA_SCTRL_PWIDTH
#define AT32_DMA_CTRL_PWIDTH_BYTE     DMA_SCTRL_PWIDTH_8BITS
#define AT32_DMA_CTRL_PWIDTH_HWORD    DMA_SCTRL_PWIDTH_16BITS
#define AT32_DMA_CTRL_PWIDTH_WORD     DMA_SCTRL_PWIDTH_32BITS
#define AT32_DMA_CTRL_MWIDTH_MASK     DMA_SCTRL_MWIDTH
#define AT32_DMA_CTRL_MWIDTH_BYTE     DMA_SCTRL_MWIDTH_8BITS
#define AT32_DMA_CTRL_MWIDTH_HWORD    DMA_SCTRL_MWIDTH_16BITS
#define AT32_DMA_CTRL_MWIDTH_WORD     DMA_SCTRL_MWIDTH_32BITS
#define AT32_DMA_CTRL_WIDTH_MASK      (AT32_DMA_CTRL_PWIDTH_MASK |              \
                                       AT32_DMA_CTRL_MWIDTH_MASK)
#define AT32_DMA_CTRL_CHPL_MASK       DMA_SCTRL_SPL
#define AT32_DMA_CTRL_CHPL(n)         ((n) << 16U)
/** @} */

/**
 * @name    CTRL register constants only found in DMAv2
 * @{
 */
#define AT32_DMA_CTRL_DMERRIEN        DMA_SCTRL_DMERRIEN
#define AT32_DMA_CTRL_PFCTRL          DMA_SCTRL_PFCTRL
#define AT32_DMA_CTRL_PINCMOS         DMA_SCTRL_PINCOS
#define AT32_DMA_CTRL_DMM             DMA_SCTRL_DMM
#define AT32_DMA_CTRL_CM              DMA_SCTRL_CM
#define AT32_DMA_CTRL_PBURST_MASK     DMA_SCTRL_PBURST
#define AT32_DMA_CTRL_PBURST_SINGLE   DMA_SCTRL_PBURST_SINGLE
#define AT32_DMA_CTRL_PBURST_INCR4    DMA_SCTRL_PBURST_INCR4
#define AT32_DMA_CTRL_PBURST_INCR8    DMA_SCTRL_PBURST_INCR8
#define AT32_DMA_CTRL_PBURST_INCR16   DMA_SCTRL_PBURST_INCR16
#define AT32_DMA_CTRL_MBURST_MASK     DMA_SCTRL_MBURST
#define AT32_DMA_CTRL_MBURST_SINGLE   DMA_SCTRL_MBURST_SINGLE
#define AT32_DMA_CTRL_MBURST_INCR4    DMA_SCTRL_MBURST_INCR4
#define AT32_DMA_CTRL_MBURST_INCR8    DMA_SCTRL_MBURST_INCR8
#define AT32_DMA_CTRL_MBURST_INCR16   DMA_SCTRL_MBURST_INCR16
#if (AT32_DMA_SUPPORTS_DMAMUX == FALSE) || defined(__DOXYGEN__)
#define AT32_DMA_CTRL_CHSEL_MASK      DMA_SCTRL_CHSEL
#define AT32_DMA_CTRL_CHSEL(n)        ((n) << 25U)
#else
#define AT32_DMA_CTRL_CHSEL_MASK      0U
#define AT32_DMA_CTRL_CHSEL(n)        0U
#endif
/** @} */

/**
 * @name    FCTRL register constants only found in DMAv2
 * @{
 */
#define AT32_DMA_FCTRL_RESET_VALUE   0x00000021U
#define AT32_DMA_FCTRL_FERRIEN       DMA_SFCTRL_FERRIEN
#define AT32_DMA_FCTRL_FSTS_MASK     DMA_SFCTRL_FSTS
#define AT32_DMA_FCTRL_FEN           DMA_SFCTRL_FEN
#define AT32_DMA_FCTRL_FTHSEL_MASK   DMA_SFCTRL_FTHSEL
#define AT32_DMA_FCTRL_FTHSEL_1Q     DMA_SFCTRL_FTHSEL_1Q
#define AT32_DMA_FCTRL_FTHSEL_HALF   DMA_SFCTRL_FTHSEL_HALF
#define AT32_DMA_FCTRL_FTHSEL_3Q     DMA_SFCTRL_FTHSEL_3Q
#define AT32_DMA_FCTRL_FTHSEL_FULL   DMA_SFCTRL_FTHSEL_FULL
/** @} */

/**
 * @name    Status flags passed to the ISR callbacks
 */
#define AT32_DMA_STS_FERRF           DMA_STS1_FERRF1
#define AT32_DMA_STS_DMERRF          DMA_STS1_DMERRF1
#define AT32_DMA_STS_DTERRF          DMA_STS1_DTERRF1
#define AT32_DMA_STS_HDTF            DMA_STS1_HDTF1
#define AT32_DMA_STS_FDTF            DMA_STS1_FDTF1
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if !defined(AT32_DMA_SUPPORTS_DMAMUX)
#error "AT32_DMA_SUPPORTS_DMAMUX not defined in registry"
#endif

#if !defined(AT32_HAS_DMA1)
#error "AT32_HAS_DMA1 missing in registry"
#endif

#if !defined(AT32_HAS_DMA2)
#error "AT32_HAS_DMA2 missing in registry"
#endif

#if !defined(AT32_DMA1_CH1_HANDLER)
#error "AT32_DMA1_CH1_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA1_CH2_HANDLER)
#error "AT32_DMA1_CH2_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA1_CH3_HANDLER)
#error "AT32_DMA1_CH3_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA1_CH4_HANDLER)
#error "AT32_DMA1_CH4_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA1_CH5_HANDLER)
#error "AT32_DMA1_CH5_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA1_CH6_HANDLER)
#error "AT32_DMA1_CH6_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA1_CH7_HANDLER)
#error "AT32_DMA1_CH7_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA1_CH8_HANDLER)
#error "AT32_DMA1_CH8_HANDLER missing in registry"
#endif

#if AT32_HAS_DMA2 == TRUE

#if !defined(AT32_DMA2_CH1_HANDLER)
#error "AT32_DMA2_CH1_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA2_CH2_HANDLER)
#error "AT32_DMA2_CH2_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA2_CH3_HANDLER)
#error "AT32_DMA2_CH3_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA2_CH4_HANDLER)
#error "AT32_DMA2_CH4_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA2_CH5_HANDLER)
#error "AT32_DMA2_CH5_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA2_CH6_HANDLER)
#error "AT32_DMA2_CH6_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA2_CH7_HANDLER)
#error "AT32_DMA2_CH7_HANDLER missing in registry"
#endif

#if !defined(AT32_DMA2_CH8_HANDLER)
#error "AT32_DMA2_CH8_HANDLER missing in registry"
#endif

#endif

#if !defined(AT32_DMA1_CH1_NUMBER)
#error "AT32_DMA1_CH1_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA1_CH2_NUMBER)
#error "AT32_DMA1_CH2_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA1_CH3_NUMBER)
#error "AT32_DMA1_CH3_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA1_CH4_NUMBER)
#error "AT32_DMA1_CH4_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA1_CH5_NUMBER)
#error "AT32_DMA1_CH5_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA1_CH6_NUMBER)
#error "AT32_DMA1_CH6_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA1_CH7_NUMBER)
#error "AT32_DMA1_CH7_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA1_CH8_NUMBER)
#error "AT32_DMA1_CH8_NUMBER missing in registry"
#endif

#if AT32_HAS_DMA2 == TRUE

#if !defined(AT32_DMA2_CH1_NUMBER)
#error "AT32_DMA2_CH1_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA2_CH2_NUMBER)
#error "AT32_DMA2_CH2_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA2_CH3_NUMBER)
#error "AT32_DMA2_CH3_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA2_CH4_NUMBER)
#error "AT32_DMA2_CH4_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA2_CH5_NUMBER)
#error "AT32_DMA2_CH5_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA2_CH6_NUMBER)
#error "AT32_DMA2_CH6_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA2_CH7_NUMBER)
#error "AT32_DMA2_CH7_NUMBER missing in registry"
#endif

#if !defined(AT32_DMA2_CH8_NUMBER)
#error "AT32_DMA2_CH8_NUMBER missing in registry"
#endif

#endif

#if (AT32_DMA_SUPPORTS_DMAMUX == TRUE) || defined(__DOXYGEN__)
#include "at32_dmamux.h"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   AT32 DMA STS function type.
 *
 * @param[in] p         parameter for the registered function
 * @param[in] flags     pre-shifted content of the xSTS register, the bits
 *                      are aligned to bit zero
 */
typedef void (*at32_dmasts_t)(void *p, uint32_t flags);

/**
 * @brief   AT32 DMA stream descriptor structure.
 */
typedef struct {
  DMA_Stream_TypeDef     *stream;        /**< @brief Associated DMA stream.  */
  volatile uint32_t       *clr;           /**< @brief Associated CLR reg.     */
#if (AT32_DMA_SUPPORTS_DMAMUX == TRUE) || defined(__DOXYGEN__)
  DMAMUX_Channel_TypeDef *mux;           /**< @brief Associated DMA mux.      */
#else
  uint8_t                 dummy;          /**< @brief Filler.                 */
#endif
  uint8_t                 shift;          /**< @brief Bits offset in xCLR
                                              register.                      */
  uint8_t                 selfindex;      /**< @brief Index to self in array. */
  uint8_t                 vector;         /**< @brief Associated IRQ vector.  */
} at32_dma_stream_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @name    Macro Functions
 * @{
 */
/**
 * @brief   Associates a peripheral data register to a DMA stream.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 * @param[in] addr      value to be written in the PADDR register
 *
 * @special
 */
#define dmaStreamSetPeripheral(dmastp, addr) {                              \
  (dmastp)->stream->PADDR  = (uint32_t)(addr);                              \
}

/**
 * @brief   Associates a memory destination to a DMA stream.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 * @param[in] addr      value to be written in the M0ADDR register
 *
 * @special
 */
#define dmaStreamSetMemory0(dmastp, addr) {                                 \
  (dmastp)->stream->M0ADDR  = (uint32_t)(addr);                             \
}

/**
 * @brief   Associates an alternate memory destination to a DMA stream.
 * @note    This function can be invoked in both ISR or thread context.
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 * @param[in] addr      value to be written in the M1ADDR register
 *
 * @special
 */
#define dmaStreamSetMemory1(dmastp, addr) {                                 \
  (dmastp)->stream->M1ADDR  = (uint32_t)(addr);                             \
}

/**
 * @brief   Sets the number of transfers to be performed.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 * @param[in] size      value to be written in the DTCNT register
 *
 * @special
 */
#define dmaStreamSetTransactionSize(dmastp, size) {                         \
  (dmastp)->stream->DTCNT  = (uint32_t)(size);                              \
}

/**
 * @brief   Returns the number of transfers to be performed.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 * @return              The number of transfers to be performed.
 *
 * @special
 */
#define dmaStreamGetTransactionSize(dmastp) ((size_t)((dmastp)->stream->DTCNT))

/**
 * @brief   Programs the stream mode settings.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 * @param[in] mode      value to be written in the CTRL register
 *
 * @special
 */
#define dmaStreamSetMode(dmastp, mode) {                                    \
  (dmastp)->stream->CTRL  = (uint32_t)(mode);                               \
}

/**
 * @brief   Programs the stream FIFO settings.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 * @param[in] mode      value to be written in the FCTRL register
 *
 * @special
 */
#define dmaStreamSetFIFO(dmastp, mode) {                                    \
  (dmastp)->stream->FCTRL = (uint32_t)(mode);                                 \
}

/**
 * @brief   DMA stream enable.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 *
 * @special
 */
#define dmaStreamEnable(dmastp) {                                           \
  (dmastp)->stream->CTRL |= AT32_DMA_CTRL_SEN;                                  \
}

/**
 * @brief   DMA stream disable.
 * @details The function disables the specified stream, waits for the disable
 *          operation to complete and then clears any pending interrupt.
 * @note    This function can be invoked in both ISR or thread context.
 * @note    Interrupts enabling flags are set to zero after this call, see
 *          bug 3607518.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 *
 * @special
 */
#define dmaStreamDisable(dmastp) {                                              \
  (dmastp)->stream->CTRL &= ~(AT32_DMA_CTRL_FDTIEN   | AT32_DMA_CTRL_HDTIEN   | \
                              AT32_DMA_CTRL_DTERRIEN | AT32_DMA_CTRL_DMERRIEN | \
                              AT32_DMA_CTRL_SEN);                               \
  while (((dmastp)->stream->CTRL & AT32_DMA_CTRL_SEN) != 0)                     \
    ;                                                                           \
  dmaStreamClearInterrupt(dmastp);                                              \
}

/**
 * @brief   DMA stream interrupt sources clear.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 *
 * @special
 */
#define dmaStreamClearInterrupt(dmastp) {                                   \
  *(dmastp)->clr = AT32_DMA_STS_MASK << (dmastp)->shift;                    \
}

/**
 * @brief   Starts a memory to memory operation using the specified stream.
 * @note    The default transfer data mode is "byte to byte" but it can be
 *          changed by specifying extra options in the @p mode parameter.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 * @param[in] mode      value to be written in the CCTRL register, this value
 *                      is implicitly ORed with:
 *                      - @p AT32_DMA_CTRL_MINCM
 *                      - @p AT32_DMA_CTRL_PINCM
 *                      - @p AT32_DMA_CTRL_DTD_M2M
 *                      - @p AT32_DMA_CTRL_SEN
 *                      .
 * @param[in] src       source address
 * @param[in] dst       destination address
 * @param[in] n         number of data units to copy
 */
#define dmaStartMemCopy(dmastp, mode, src, dst, n) {                        \
  dmaStreamSetPeripheral(dmastp, src);                                      \
  dmaStreamSetMemory0(dmastp, dst);                                         \
  dmaStreamSetTransactionSize(dmastp, n);                                   \
  dmaStreamSetMode(dmastp, (mode) |                                         \
                           AT32_DMA_CTRL_MINCM | AT32_DMA_CTRL_PINCM |      \
                           AT32_DMA_CTRL_DTD_M2M);                          \
  dmaStreamEnable(dmastp);                                                  \
}

/**
 * @brief   Polled wait for DMA transfer end.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 */
#define dmaWaitCompletion(dmastp) {                                             \
  (dmastp)->stream->CTRL &= ~(AT32_DMA_CTRL_FDTIEN   | AT32_DMA_CTRL_HDTIEN  |  \
                              AT32_DMA_CTRL_DTERRIEN | AT32_DMA_CTRL_DMERRIEN); \
  while ((dmastp)->stream->CTRL & AT32_DMA_CTRL_SEN)                            \
    ;                                                                           \
  dmaStreamClearInterrupt(dmastp);                                              \
}

/**
 * @brief   DMA stream current target.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p dmaStreamAlloc().
 * @post    After use the stream can be released using @p dmaStreamFree().
 *
 * @param[in] dmastp    pointer to a at32_dma_stream_t structure
 * @return              Current memory target index.
 *
 * @special
 */
#define dmaStreamGetCurrentTarget(dmastp)                                     \
  (((dmastp)->stream->CTRL >> 19U) & 1U)
/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
extern const at32_dma_stream_t _at32_dma_streams[AT32_DMA_STREAMS];
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void dmaInit(void);
  const at32_dma_stream_t *dmaStreamAllocI(uint32_t id,
                                           uint32_t priority,
                                           at32_dmasts_t func,
                                           void *param);
  const at32_dma_stream_t *dmaStreamAlloc(uint32_t id,
                                          uint32_t priority,
                                          at32_dmasts_t func,
                                          void *param);
  void dmaStreamFreeI(const at32_dma_stream_t *dmastp);
  void dmaStreamFree(const at32_dma_stream_t *dmastp);
#if AT32_DMA_SUPPORTS_DMAMUX == TRUE
  void dmaSetRequestSource(const at32_dma_stream_t *dmastp, uint32_t per);
#endif
#ifdef __cplusplus
}
#endif

#endif /* AT32_DMA_H */

/** @} */
