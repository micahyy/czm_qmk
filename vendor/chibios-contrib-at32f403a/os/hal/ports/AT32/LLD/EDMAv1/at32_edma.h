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
 * @file    EDMAv1/at32_edma.h
 * @brief   Enhanced-EDMA helper driver header.
 *
 * @addtogroup AT32_EDMA
 * @{
 */

#ifndef AT32_EDMA_H
#define AT32_EDMA_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief   Requires use of EDMAv1 driver model.
 */
#define AT32_USE_EDMA_V1            TRUE

/**
 * @brief   EDMA capability.
 * @details if @p TRUE then the EDMA is able of burst transfers, FIFOs,
 *          scatter gather and other advanced features.
 */
#define AT32_EDMA_ADVANCED          TRUE

/**
 * @brief   Total number of EDMA streams.
 * @details This is the total number of streams among all the EDMA units.
 */
#if AT32_HAS_EDMA2 == TRUE
#define AT32_EDMA_STREAMS           16U
#else
#define AT32_EDMA_STREAMS           8U
#endif

/**
 * @brief   Mask of the STS bits passed to the EDMA callback functions.
 */
#define AT32_EDMA_STS_MASK          0x3DU

/**
 * @brief   Returns the channel associated to the specified stream.
 *
 * @param[in] id        the unique numeric stream identifier
 * @param[in] c         a stream/channel association word, one channel per
 *                      nibble
 * @return              Returns the channel associated to the stream.
 */
#define AT32_EDMA_GETCHANNEL(id, c) (((c) >> (((id) & 7U) * 4U)) & 15U)

/**
 * @brief   Checks if a EDMA priority is within the valid range.
 * @param[in] prio      EDMA priority
 *
 * @retval              The check result.
 * @retval false        invalid EDMA priority.
 * @retval true         correct EDMA priority.
 */
#define AT32_EDMA_IS_VALID_PRIORITY(prio) (((prio) >= 0U) && ((prio) <= 3U))

#if (AT32_EDMA_SUPPORTS_EDMAMUX == FALSE) || defined(_DOXYGEN__)
/**
 * @brief   Checks if a EDMA stream id is within the valid range.
 *
 * @param[in] id        EDMA stream id
 * @retval              The check result.
 * @retval false        invalid EDMA stream.
 * @retval true         correct EDMA stream.
 */
#define AT32_EDMA_IS_VALID_STREAM(id) (((id) >= 0U) &&                      \
                                       ((id) < AT32_EDMA_STREAMS))
#else /* AT32_EDMA_SUPPORTS_EDMAMUX == FALSE */
#if AT32_HAS_EDMA2 == TRUE
#define AT32_EDMA_IS_VALID_STREAM(id) (((id) >= 0U) &&                      \
                                       ((id) <= (AT32_EDMA_STREAMS + 2)))
#else
#define AT32_EDMA_IS_VALID_STREAM(id) (((id) >= 0U) &&                      \
                                       ((id) <= (AT32_EDMA_STREAMS + 1)))
#endif
#endif /* AT32_EDMA_SUPPORTS_EDMAMUX == FALSE */

/**
 * @brief   Returns an unique numeric identifier for a EDMA stream.
 *
 * @param[in] edma       the EDMA unit number
 * @param[in] stream    the stream number
 * @return              An unique numeric stream identifier.
 */
#define AT32_EDMA_STREAM_ID(edma, stream) ((((edma) - 1U) * 8U) + ((stream) - 1))

/**
 * @brief   Returns a EDMA stream identifier mask.
 *
 *
 * @param[in] edma       the EDMA unit number
 * @param[in] stream    the stream number
 * @return              A EDMA stream identifier mask.
 */
#define AT32_EDMA_STREAM_ID_MSK(edma, stream)                                \
  (1U << AT32_EDMA_STREAM_ID(edma, stream))

/**
 * @brief   Checks if a EDMA stream unique identifier belongs to a mask.
 * @param[in] id        the stream numeric identifier
 * @param[in] mask      the stream numeric identifiers mask
 *
 * @retval              The check result.
 * @retval false        id does not belong to the mask.
 * @retval true         id belongs to the mask.
 */
#define AT32_EDMA_IS_VALID_ID(id, mask) (((1U << (id)) & (mask)))

#if (AT32_EDMA_SUPPORTS_EDMAMUX == TRUE) || defined(_DOXYGEN__)
/**
 * @name    Special stream identifiers
 * @{
 */
#define AT32_EDMA_STREAM_ID_ANY           AT32_EDMA_STREAMS
#define AT32_EDMA_STREAM_ID_ANY_EDMA1    (AT32_EDMA_STREAM_ID_ANY + 1)
#if AT32_HAS_EDMA2 == TRUE
#define AT32_EDMA_STREAM_ID_ANY_EDMA2    (AT32_EDMA_STREAM_ID_ANY_EDMA1 + 1)
#endif
/** @} */
#endif

/**
 * @name    EDMA streams identifiers
 * @{
 */
/**
 * @brief   Returns a pointer to a at32_edma_stream_t structure.
 *
 * @param[in] id        the stream numeric identifier
 * @return              A pointer to the at32_edma_stream_t constant structure
 *                      associated to the EDMA stream.
 */
#define AT32_EDMA_STREAM(id)        (&_at32_edma_streams[id])

#define AT32_EDMA1_STREAM1          AT32_EDMA_STREAM(0)
#define AT32_EDMA1_STREAM2          AT32_EDMA_STREAM(1)
#define AT32_EDMA1_STREAM3          AT32_EDMA_STREAM(2)
#define AT32_EDMA1_STREAM4          AT32_EDMA_STREAM(3)
#define AT32_EDMA1_STREAM5          AT32_EDMA_STREAM(4)
#define AT32_EDMA1_STREAM6          AT32_EDMA_STREAM(5)
#define AT32_EDMA1_STREAM7          AT32_EDMA_STREAM(6)
#define AT32_EDMA1_STREAM8          AT32_EDMA_STREAM(7)
#if AT32_HAS_EDMA2 == TRUE
#define AT32_EDMA2_STREAM1          AT32_EDMA_STREAM(8)
#define AT32_EDMA2_STREAM2          AT32_EDMA_STREAM(9)
#define AT32_EDMA2_STREAM3          AT32_EDMA_STREAM(10)
#define AT32_EDMA2_STREAM4          AT32_EDMA_STREAM(11)
#define AT32_EDMA2_STREAM5          AT32_EDMA_STREAM(12)
#define AT32_EDMA2_STREAM6          AT32_EDMA_STREAM(13)
#define AT32_EDMA2_STREAM7          AT32_EDMA_STREAM(14)
#define AT32_EDMA2_STREAM8          AT32_EDMA_STREAM(15)
#endif
/** @} */

/**
 * @name    CTRL register constants common to all EDMA types
 * @{
 */
#define AT32_EDMA_SCTRL_RESET_VALUE    0x00000000U
#define AT32_EDMA_CTRL_SEN             EDMA_SCTRL_SEN
#define AT32_EDMA_CTRL_DTERRIEN        EDMA_SCTRL_DTERRIEN
#define AT32_EDMA_CTRL_HDTIEN          EDMA_SCTRL_HDTIEN
#define AT32_EDMA_CTRL_FDTIEN          EDMA_SCTRL_FDTIEN
#define AT32_EDMA_CTRL_DTD_MASK        EDMA_SCTRL_DTD
#define AT32_EDMA_CTRL_DTD_P2M         0U
#define AT32_EDMA_CTRL_DTD_M2P         EDMA_SCTRL_DTD_M2P
#define AT32_EDMA_CTRL_DTD_M2M         EDMA_SCTRL_DTD_M2M
#define AT32_EDMA_CTRL_LM              EDMA_SCTRL_LM
#define AT32_EDMA_CTRL_PINCM           EDMA_SCTRL_PINCM
#define AT32_EDMA_CTRL_MINCM           EDMA_SCTRL_MINCM
#define AT32_EDMA_CTRL_PWIDTH_MASK     EDMA_SCTRL_PWIDTH
#define AT32_EDMA_CTRL_PWIDTH_BYTE     EDMA_SCTRL_PWIDTH_8BITS
#define AT32_EDMA_CTRL_PWIDTH_HWORD    EDMA_SCTRL_PWIDTH_16BITS
#define AT32_EDMA_CTRL_PWIDTH_WORD     EDMA_SCTRL_PWIDTH_32BITS
#define AT32_EDMA_CTRL_MWIDTH_MASK     EDMA_SCTRL_MWIDTH
#define AT32_EDMA_CTRL_MWIDTH_BYTE     EDMA_SCTRL_MWIDTH_8BITS
#define AT32_EDMA_CTRL_MWIDTH_HWORD    EDMA_SCTRL_MWIDTH_16BITS
#define AT32_EDMA_CTRL_MWIDTH_WORD     EDMA_SCTRL_MWIDTH_32BITS
#define AT32_EDMA_CTRL_WIDTH_MASK      (AT32_EDMA_CTRL_PWIDTH_MASK |              \
                                        AT32_EDMA_CTRL_MWIDTH_MASK)
#define AT32_EDMA_CTRL_CHPL_MASK       EDMA_SCTRL_SPL
#define AT32_EDMA_CTRL_CHPL(n)         ((n) << 16U)
/** @} */

/**
 * @name    CTRL register constants only found in EDMAv2
 * @{
 */
#define AT32_EDMA_CTRL_DMERRIEN        EDMA_SCTRL_DMERRIEN
#define AT32_EDMA_CTRL_PFCTRL          EDMA_SCTRL_PFCTRL
#define AT32_EDMA_CTRL_PINCMOS         EDMA_SCTRL_PINCOS
#define AT32_EDMA_CTRL_DMM             EDMA_SCTRL_DMM
#define AT32_EDMA_CTRL_CM              EDMA_SCTRL_CM
#define AT32_EDMA_CTRL_PBURST_MASK     EDMA_SCTRL_PBURST
#define AT32_EDMA_CTRL_PBURST_SINGLE   EDMA_SCTRL_PBURST_SINGLE
#define AT32_EDMA_CTRL_PBURST_INCR4    EDMA_SCTRL_PBURST_INCR4
#define AT32_EDMA_CTRL_PBURST_INCR8    EDMA_SCTRL_PBURST_INCR8
#define AT32_EDMA_CTRL_PBURST_INCR16   EDMA_SCTRL_PBURST_INCR16
#define AT32_EDMA_CTRL_MBURST_MASK     EDMA_SCTRL_MBURST
#define AT32_EDMA_CTRL_MBURST_SINGLE   EDMA_SCTRL_MBURST_SINGLE
#define AT32_EDMA_CTRL_MBURST_INCR4    EDMA_SCTRL_MBURST_INCR4
#define AT32_EDMA_CTRL_MBURST_INCR8    EDMA_SCTRL_MBURST_INCR8
#define AT32_EDMA_CTRL_MBURST_INCR16   EDMA_SCTRL_MBURST_INCR16
#if (AT32_EDMA_SUPPORTS_EDMAMUX == FALSE) || defined(__DOXYGEN__)
#define AT32_EDMA_CTRL_CHSEL_MASK      EDMA_SCTRL_CHSEL
#define AT32_EDMA_CTRL_CHSEL(n)        ((n) << 25U)
#else
#define AT32_EDMA_CTRL_CHSEL_MASK      0U
#define AT32_EDMA_CTRL_CHSEL(n)        0U
#endif
/** @} */

/**
 * @name    FCTRL register constants only found in EDMAv2
 * @{
 */
#define AT32_EDMA_FCTRL_RESET_VALUE   0x00000021U
#define AT32_EDMA_FCTRL_FERRIEN       EDMA_SFCTRL_FERRIEN
#define AT32_EDMA_FCTRL_FSTS_MASK     EDMA_SFCTRL_FSTS
#define AT32_EDMA_FCTRL_FEN           EDMA_SFCTRL_FEN
#define AT32_EDMA_FCTRL_FTHSEL_MASK   EDMA_SFCTRL_FTHSEL
#define AT32_EDMA_FCTRL_FTHSEL_1Q     EDMA_SFCTRL_FTHSEL_1Q
#define AT32_EDMA_FCTRL_FTHSEL_HALF   EDMA_SFCTRL_FTHSEL_HALF
#define AT32_EDMA_FCTRL_FTHSEL_3Q     EDMA_SFCTRL_FTHSEL_3Q
#define AT32_EDMA_FCTRL_FTHSEL_FULL   EDMA_SFCTRL_FTHSEL_FULL
/** @} */

/**
 * @name    Status flags passed to the ISR callbacks
 */
#define AT32_EDMA_STS_FERRF           EDMA_STS1_FERRF1
#define AT32_EDMA_STS_DMERRF          EDMA_STS1_DMERRF1
#define AT32_EDMA_STS_DTERRF          EDMA_STS1_DTERRF1
#define AT32_EDMA_STS_HDTF            EDMA_STS1_HDTF1
#define AT32_EDMA_STS_FDTF            EDMA_STS1_FDTF1
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if !defined(AT32_EDMA_SUPPORTS_EDMAMUX)
#error "AT32_EDMA_SUPPORTS_EDMAMUX not defined in registry"
#endif

#if !defined(AT32_HAS_EDMA1)
#error "AT32_HAS_EDMA1 missing in registry"
#endif

#if !defined(AT32_HAS_EDMA2)
#error "AT32_HAS_EDMA2 missing in registry"
#endif

#if !defined(AT32_EDMA1_CH1_HANDLER)
#error "AT32_EDMA1_CH1_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH2_HANDLER)
#error "AT32_EDMA1_CH2_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH3_HANDLER)
#error "AT32_EDMA1_CH3_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH4_HANDLER)
#error "AT32_EDMA1_CH4_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH5_HANDLER)
#error "AT32_EDMA1_CH5_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH6_HANDLER)
#error "AT32_EDMA1_CH6_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH7_HANDLER)
#error "AT32_EDMA1_CH7_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH8_HANDLER)
#error "AT32_EDMA1_CH8_HANDLER missing in registry"
#endif

#if AT32_HAS_EDMA2 == TRUE

#if !defined(AT32_EDMA2_CH1_HANDLER)
#error "AT32_EDMA2_CH1_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH2_HANDLER)
#error "AT32_EDMA2_CH2_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH3_HANDLER)
#error "AT32_EDMA2_CH3_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH4_HANDLER)
#error "AT32_EDMA2_CH4_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH5_HANDLER)
#error "AT32_EDMA2_CH5_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH6_HANDLER)
#error "AT32_EDMA2_CH6_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH7_HANDLER)
#error "AT32_EDMA2_CH7_HANDLER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH8_HANDLER)
#error "AT32_EDMA2_CH8_HANDLER missing in registry"
#endif

#endif

#if !defined(AT32_EDMA1_CH1_NUMBER)
#error "AT32_EDMA1_CH1_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH2_NUMBER)
#error "AT32_EDMA1_CH2_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH3_NUMBER)
#error "AT32_EDMA1_CH3_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH4_NUMBER)
#error "AT32_EDMA1_CH4_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH5_NUMBER)
#error "AT32_EDMA1_CH5_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH6_NUMBER)
#error "AT32_EDMA1_CH6_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH7_NUMBER)
#error "AT32_EDMA1_CH7_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA1_CH8_NUMBER)
#error "AT32_EDMA1_CH8_NUMBER missing in registry"
#endif

#if AT32_HAS_EDMA2 == TRUE

#if !defined(AT32_EDMA2_CH1_NUMBER)
#error "AT32_EDMA2_CH1_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH2_NUMBER)
#error "AT32_EDMA2_CH2_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH3_NUMBER)
#error "AT32_EDMA2_CH3_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH4_NUMBER)
#error "AT32_EDMA2_CH4_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH5_NUMBER)
#error "AT32_EDMA2_CH5_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH6_NUMBER)
#error "AT32_EDMA2_CH6_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH7_NUMBER)
#error "AT32_EDMA2_CH7_NUMBER missing in registry"
#endif

#if !defined(AT32_EDMA2_CH8_NUMBER)
#error "AT32_EDMA2_CH8_NUMBER missing in registry"
#endif

#endif

#if (AT32_EDMA_SUPPORTS_EDMAMUX == TRUE) || defined(__DOXYGEN__)
#include "at32_edmamux.h"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   AT32 EDMA STS function type.
 *
 * @param[in] p         parameter for the registered function
 * @param[in] flags     pre-shifted content of the xSTS register, the bits
 *                      are aligned to bit zero
 */
typedef void (*at32_edmasts_t)(void *p, uint32_t flags);

/**
 * @brief   AT32 EDMA stream descriptor structure.
 */
typedef struct {
  EDMA_Stream_TypeDef     *stream;        /**< @brief Associated EDMA stream.  */
  volatile uint32_t       *clr;           /**< @brief Associated CLR reg.     */
#if (AT32_EDMA_SUPPORTS_EDMAMUX == TRUE) || defined(__DOXYGEN__)
  EDMAMUX_Channel_TypeDef *mux;           /**< @brief Associated EDMA mux.      */
#else
  uint8_t                 dummy;          /**< @brief Filler.                 */
#endif
  uint8_t                 shift;          /**< @brief Bits offset in xCLR
                                              register.                      */
  uint8_t                 selfindex;      /**< @brief Index to self in array. */
  uint8_t                 vector;         /**< @brief Associated IRQ vector.  */
} at32_edma_stream_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @name    Macro Functions
 * @{
 */
/**
 * @brief   Associates a peripheral data register to a EDMA stream.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 * @param[in] addr      value to be written in the PADDR register
 *
 * @special
 */
#define edmaStreamSetPeripheral(edmastp, addr) {                              \
  (edmastp)->stream->PADDR  = (uint32_t)(addr);                              \
}

/**
 * @brief   Associates a memory destination to a EDMA stream.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 * @param[in] addr      value to be written in the M0ADDR register
 *
 * @special
 */
#define edmaStreamSetMemory0(edmastp, addr) {                                 \
  (edmastp)->stream->M0ADDR  = (uint32_t)(addr);                             \
}

/**
 * @brief   Associates an alternate memory destination to a EDMA stream.
 * @note    This function can be invoked in both ISR or thread context.
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 * @param[in] addr      value to be written in the M1ADDR register
 *
 * @special
 */
#define edmaStreamSetMemory1(edmastp, addr) {                                 \
  (edmastp)->stream->M1ADDR  = (uint32_t)(addr);                             \
}

/**
 * @brief   Sets the number of transfers to be performed.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 * @param[in] size      value to be written in the DTCNT register
 *
 * @special
 */
#define edmaStreamSetTransactionSize(edmastp, size) {                         \
  (edmastp)->stream->DTCNT  = (uint32_t)(size);                              \
}

/**
 * @brief   Returns the number of transfers to be performed.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 * @return              The number of transfers to be performed.
 *
 * @special
 */
#define edmaStreamGetTransactionSize(edmastp) ((size_t)((edmastp)->stream->DTCNT))

/**
 * @brief   Programs the stream mode settings.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 * @param[in] mode      value to be written in the CTRL register
 *
 * @special
 */
#define edmaStreamSetMode(edmastp, mode) {                                    \
  (edmastp)->stream->CTRL  = (uint32_t)(mode);                               \
}

/**
 * @brief   Programs the stream FIFO settings.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 * @param[in] mode      value to be written in the FCTRL register
 *
 * @special
 */
#define edmaStreamSetFIFO(edmastp, mode) {                                    \
  (edmastp)->stream->FCTRL = (uint32_t)(mode);                                 \
}

/**
 * @brief   EDMA stream enable.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 *
 * @special
 */
#define edmaStreamEnable(edmastp) {                                           \
  (edmastp)->stream->CTRL |= AT32_EDMA_CTRL_SEN;                                  \
}

/**
 * @brief   EDMA stream disable.
 * @details The function disables the specified stream, waits for the disable
 *          operation to complete and then clears any pending interrupt.
 * @note    This function can be invoked in both ISR or thread context.
 * @note    Interrupts enabling flags are set to zero after this call, see
 *          bug 3607518.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 *
 * @special
 */
#define edmaStreamDisable(edmastp) {                                               \
  (edmastp)->stream->CTRL &= ~(AT32_EDMA_CTRL_FDTIEN   | AT32_EDMA_CTRL_HDTIEN   | \
                               AT32_EDMA_CTRL_DTERRIEN | AT32_EDMA_CTRL_DMERRIEN | \
                               AT32_EDMA_CTRL_SEN);                                \
  while (((edmastp)->stream->CTRL & AT32_EDMA_CTRL_SEN) != 0)                      \
    ;                                                                              \
  edmaStreamClearInterrupt(edmastp);                                               \
}

/**
 * @brief   EDMA stream interrupt sources clear.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 *
 * @special
 */
#define edmaStreamClearInterrupt(edmastp) {                                      \
  *(edmastp)->clr = AT32_EDMA_STS_MASK << (edmastp)->shift;                      \
}

/**
 * @brief   Starts a memory to memory operation using the specified stream.
 * @note    The default transfer data mode is "byte to byte" but it can be
 *          changed by specifying extra options in the @p mode parameter.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 * @param[in] mode      value to be written in the CCTRL register, this value
 *                      is implicitly ORed with:
 *                      - @p AT32_EDMA_CTRL_MINCM
 *                      - @p AT32_EDMA_CTRL_PINCM
 *                      - @p AT32_EDMA_CTRL_DTD_M2M
 *                      - @p AT32_EDMA_CTRL_SEN
 *                      .
 * @param[in] src       source address
 * @param[in] dst       destination address
 * @param[in] n         number of data units to copy
 */
#define edmaStartMemCopy(edmastp, mode, src, dst, n) {                        \
  edmaStreamSetPeripheral(edmastp, src);                                      \
  edmaStreamSetMemory0(edmastp, dst);                                         \
  edmaStreamSetTransactionSize(edmastp, n);                                   \
  edmaStreamSetMode(edmastp, (mode) |                                         \
                             AT32_EDMA_CTRL_MINCM | AT32_EDMA_CTRL_PINCM |    \
                             AT32_EDMA_CTRL_DTD_M2M);                         \
  edmaStreamEnable(edmastp);                                                  \
}

/**
 * @brief   Polled wait for EDMA transfer end.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 */
#define edmaWaitCompletion(edmastp) {                                              \
  (edmastp)->stream->CTRL &= ~(AT32_EDMA_CTRL_FDTIEN   | AT32_EDMA_CTRL_HDTIEN  |  \
                               AT32_EDMA_CTRL_DTERRIEN | AT32_EDMA_CTRL_DMERRIEN); \
  while ((edmastp)->stream->CTRL & AT32_EDMA_CTRL_SEN)                             \
    ;                                                                              \
  edmaStreamClearInterrupt(edmastp);                                               \
}

/**
 * @brief   EDMA stream current target.
 * @note    This function can be invoked in both ISR or thread context.
 * @pre     The stream must have been allocated using @p edmaStreamAlloc().
 * @post    After use the stream can be released using @p edmaStreamFree().
 *
 * @param[in] edmastp    pointer to a at32_edma_stream_t structure
 * @return              Current memory target index.
 *
 * @special
 */
#define edmaStreamGetCurrentTarget(edmastp)                                     \
  (((edmastp)->stream->CTRL >> 19U) & 1U)
/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
extern const at32_edma_stream_t _at32_edma_streams[AT32_EDMA_STREAMS];
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void edmaInit(void);
  const at32_edma_stream_t *edmaStreamAllocI(uint32_t id,
                                             uint32_t priority,
                                             at32_edmasts_t func,
                                             void *param);
  const at32_edma_stream_t *edmaStreamAlloc(uint32_t id,
                                            uint32_t priority,
                                            at32_edmasts_t func,
                                            void *param);
  void edmaStreamFreeI(const at32_edma_stream_t *edmastp);
  void edmaStreamFree(const at32_edma_stream_t *edmastp);
#if AT32_EDMA_SUPPORTS_EDMAMUX == TRUE
  void edmaSetRequestSource(const at32_edma_stream_t *edmastp, uint32_t per);
#endif
#ifdef __cplusplus
}
#endif

#endif /* AT32_EDMA_H */

/** @} */
