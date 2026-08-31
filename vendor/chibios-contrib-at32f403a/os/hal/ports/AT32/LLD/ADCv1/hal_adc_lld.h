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
 * @file    ADCv1/hal_adc_lld.h
 * @brief   AT32 ADC subsystem low level driver header.
 *
 * @addtogroup ADC
 * @{
 */

#ifndef HAL_ADC_LLD_H
#define HAL_ADC_LLD_H

#if HAL_USE_ADC || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    Triggers selection
 * @{
 */
#define ADC_CTRL2_OCTESEL_SRC(n)   ((n) << 17) /**< @brief Trigger source.     */
#define ADC_CTRL2_OCTESEL_OCSWTRG  (7 << 17)   /**< @brief Software trigger.   */
/** @} */

/**
 * @name    Available analog channels
 * @{
 */
#define ADC_CHANNEL_IN0         0   /**< @brief External analog input 0.    */
#define ADC_CHANNEL_IN1         1   /**< @brief External analog input 1.    */
#define ADC_CHANNEL_IN2         2   /**< @brief External analog input 2.    */
#define ADC_CHANNEL_IN3         3   /**< @brief External analog input 3.    */
#define ADC_CHANNEL_IN4         4   /**< @brief External analog input 4.    */
#define ADC_CHANNEL_IN5         5   /**< @brief External analog input 5.    */
#define ADC_CHANNEL_IN6         6   /**< @brief External analog input 6.    */
#define ADC_CHANNEL_IN7         7   /**< @brief External analog input 7.    */
#define ADC_CHANNEL_IN8         8   /**< @brief External analog input 8.    */
#define ADC_CHANNEL_IN9         9   /**< @brief External analog input 9.    */
#define ADC_CHANNEL_IN10        10  /**< @brief External analog input 10.   */
#define ADC_CHANNEL_IN11        11  /**< @brief External analog input 11.   */
#define ADC_CHANNEL_IN12        12  /**< @brief External analog input 12.   */
#define ADC_CHANNEL_IN13        13  /**< @brief External analog input 13.   */
#define ADC_CHANNEL_IN14        14  /**< @brief External analog input 14.   */
#define ADC_CHANNEL_IN15        15  /**< @brief External analog input 15.   */
#define ADC_CHANNEL_SENSOR      16  /**< @brief Internal temperature sensor.*/
#define ADC_CHANNEL_VINTRV      17  /**< @brief Internal reference.         */
#define ADC_CHANNEL_VSS         18  /**< @brief Internal reference.         */
/** @} */

/**
 * @name    Sampling rates
 * @{
 */
#define ADC_SAMPLE_1P5          0   /**< @brief 1.5 cycles sampling time.   */
#define ADC_SAMPLE_7P5          1   /**< @brief 7.5 cycles sampling time.   */
#define ADC_SAMPLE_13P5         2   /**< @brief 13.5 cycles sampling time.  */
#define ADC_SAMPLE_28P5         3   /**< @brief 28.5 cycles sampling time.  */
#define ADC_SAMPLE_41P5         4   /**< @brief 41.5 cycles sampling time.  */
#define ADC_SAMPLE_55P5         5   /**< @brief 55.5 cycles sampling time.  */
#define ADC_SAMPLE_71P5         6   /**< @brief 71.5 cycles sampling time.  */
#define ADC_SAMPLE_239P5        7   /**< @brief 239.5 cycles sampling time. */
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   ADC1 driver enable switch.
 * @details If set to @p TRUE the support for ADC1 is included.
 * @note    The default is @p TRUE.
 */
#if !defined(AT32_ADC_USE_ADC1) || defined(__DOXYGEN__)
#define AT32_ADC_USE_ADC1                  FALSE
#endif

/**
 * @brief   ADC2 driver enable switch.
 * @details If set to @p TRUE the support for ADC2 is included.
 * @note    The default is @p TRUE.
 */
#if !defined(AT32_ADC_USE_ADC2) || defined(__DOXYGEN__)
#define AT32_ADC_USE_ADC2                  FALSE
#endif

/**
 * @brief   ADC3 driver enable switch.
 * @details If set to @p TRUE the support for ADC3 is included.
 * @note    The default is @p TRUE.
 */
#if !defined(AT32_ADC_USE_ADC3) || defined(__DOXYGEN__)
#define AT32_ADC_USE_ADC3                  FALSE
#endif

/**
 * @brief   ADC1 DMA priority (0..3|lowest..highest).
 */
#if !defined(AT32_ADC_ADC1_DMA_PRIORITY) || defined(__DOXYGEN__)
#define AT32_ADC_ADC1_DMA_PRIORITY         2
#endif

/**
 * @brief   ADC2 DMA priority (0..3|lowest..highest).
 */
#if !defined(AT32_ADC_ADC2_DMA_PRIORITY) || defined(__DOXYGEN__)
#define AT32_ADC_ADC2_DMA_PRIORITY         2
#endif

/**
 * @brief   ADC3 DMA priority (0..3|lowest..highest).
 */
#if !defined(AT32_ADC_ADC3_DMA_PRIORITY) || defined(__DOXYGEN__)
#define AT32_ADC_ADC3_DMA_PRIORITY         2
#endif

/**
 * @brief   ADC1 interrupt priority level setting.
 */
#if !defined(AT32_ADC_ADC1_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define AT32_ADC_ADC1_IRQ_PRIORITY         5
#endif

/**
 * @brief   ADC2 interrupt priority level setting.
 */
#if !defined(AT32_ADC_ADC2_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define AT32_ADC_ADC2_IRQ_PRIORITY         5
#endif

/**
 * @brief   ADC3 interrupt priority level setting.
 */
#if !defined(AT32_ADC_ADC3_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define AT32_ADC_ADC3_IRQ_PRIORITY         5
#endif

#if AT32_DMA_SUPPORTS_DMAMUX && AT32_USE_DMA_V1 

/**
 * @brief   ADC1 DMA MUX setting.
 */
#if !defined(AT32_ADC_ADC1_DMAMUX_CHANNEL) || defined(__DOXYGEN__)
#define AT32_ADC_ADC1_DMAMUX_CHANNEL       1
#endif

/**
 * @brief   ADC2 DMA MUX setting.
 */
#if !defined(AT32_ADC_ADC2_DMAMUX_CHANNEL) || defined(__DOXYGEN__)
#define AT32_ADC_ADC2_DMAMUX_CHANNEL       1
#endif

/**
 * @brief   ADC3 DMA MUX setting.
 */
#if !defined(AT32_ADC_ADC3_DMAMUX_CHANNEL) || defined(__DOXYGEN__)
#define AT32_ADC_ADC3_DMAMUX_CHANNEL       1
#endif

#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if AT32_ADC_USE_ADC1 && !AT32_HAS_ADC1
#error "ADC1 not present in the selected device"
#endif

#if AT32_ADC_USE_ADC2 && !AT32_HAS_ADC2
#error "ADC2 not present in the selected device"
#endif

#if AT32_ADC_USE_ADC3 && !AT32_HAS_ADC3
#error "ADC3 not present in the selected device"
#endif

#if !AT32_ADC_USE_ADC1 && !AT32_ADC_USE_ADC2 && !AT32_ADC_USE_ADC3
#error "ADC driver activated but no ADC peripheral assigned"
#endif

#if !defined(AT32_DMA_REQUIRED)
#define AT32_DMA_REQUIRED
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   ADC sample data type.
 */
typedef uint16_t adcsample_t;

/**
 * @brief   Channels number in a conversion group.
 */
typedef uint16_t adc_channels_num_t;

/**
 * @brief   Possible ADC failure causes.
 * @note    Error codes are architecture dependent and should not relied
 *          upon.
 */
typedef enum {
  ADC_ERR_DMAFAILURE = 0                    /**< DMA operations failure.    */
} adcerror_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Low level fields of the ADC driver structure.
 */
#define adc_lld_driver_fields                                               \
  /* Pointer to the ADCx registers block.*/                                 \
  ADC_TypeDef              *adc;                                            \
  /* Pointer to associated DMA channel.*/                                   \
  const at32_dma_stream_t  *dmastp;                                         \
  /* DMA mode bit mask.*/                                                   \
  uint32_t                  dmamode

/**
 * @brief   Low level fields of the ADC configuration structure.
 */
#define adc_lld_config_fields                                               \
  /* Dummy configuration, it is not needed.*/                               \
  uint32_t                  dummy

/**
 * @brief   Low level fields of the ADC configuration structure.
 */
#define adc_lld_configuration_group_fields                                  \
  /* ADC CTRL1 register initialization data.*/                              \
  uint32_t                  ctrl1;                                          \
  /* ADC CTRL2 register initialization data. */                             \
  uint32_t                  ctrl2;                                          \
  /* ADC SPT1 register initialization data.*/                               \
  uint32_t                  spt1;                                           \
  /* ADC SPT2 register initialization data.*/                               \
  uint32_t                  spt2;                                           \
  /* ADC OSQ1 register initialization data.*/                               \
  uint32_t                  osq1;                                           \
  /* ADC OSQ2 register initialization data.*/                               \
  uint32_t                  osq2;                                           \
  /* ADC OSQ3 register initialization data.*/                               \
  uint32_t                  osq3

/**
 * @name    Sequences building helper macros
 * @{
 */
/**
 * @brief   Number of channels in a conversion sequence.
 */
#define ADC_OSQ1_NUM_CH(n)      (((n) - 1) << 20)

#define ADC_OSQ3_OSN1_N(n)       ((n) << 0)  /**< @brief 1st channel in seq. */
#define ADC_OSQ3_OSN2_N(n)       ((n) << 5)  /**< @brief 2nd channel in seq. */
#define ADC_OSQ3_OSN3_N(n)       ((n) << 10) /**< @brief 3rd channel in seq. */
#define ADC_OSQ3_OSN4_N(n)       ((n) << 15) /**< @brief 4th channel in seq. */
#define ADC_OSQ3_OSN5_N(n)       ((n) << 20) /**< @brief 5th channel in seq. */
#define ADC_OSQ3_OSN6_N(n)       ((n) << 25) /**< @brief 6th channel in seq. */

#define ADC_OSQ2_OSN7_N(n)       ((n) << 0)  /**< @brief 7th channel in seq. */
#define ADC_OSQ2_OSN8_N(n)       ((n) << 5)  /**< @brief 8th channel in seq. */
#define ADC_OSQ2_OSN9_N(n)       ((n) << 10) /**< @brief 9th channel in seq. */
#define ADC_OSQ2_OSN10_N(n)      ((n) << 15) /**< @brief 10th channel in seq.*/
#define ADC_OSQ2_OSN11_N(n)      ((n) << 20) /**< @brief 11th channel in seq.*/
#define ADC_OSQ2_OSN12_N(n)      ((n) << 25) /**< @brief 12th channel in seq.*/

#define ADC_OSQ1_OSN13_N(n)      ((n) << 0)  /**< @brief 13th channel in seq.*/
#define ADC_OSQ1_OSN14_N(n)      ((n) << 5)  /**< @brief 14th channel in seq.*/
#define ADC_OSQ1_OSN15_N(n)      ((n) << 10) /**< @brief 15th channel in seq.*/
#define ADC_OSQ1_OSN16_N(n)      ((n) << 15) /**< @brief 16th channel in seq.*/
/** @} */

/**
 * @name    Sampling rate settings helper macros
 * @{
 */
#define ADC_SPT2_CSPT0(n)       ((n) << 0)  /**< @brief AN0 sampling time.  */
#define ADC_SPT2_CSPT1(n)       ((n) << 3)  /**< @brief AN1 sampling time.  */
#define ADC_SPT2_CSPT2(n)       ((n) << 6)  /**< @brief AN2 sampling time.  */
#define ADC_SPT2_CSPT3(n)       ((n) << 9)  /**< @brief AN3 sampling time.  */
#define ADC_SPT2_CSPT4(n)       ((n) << 12) /**< @brief AN4 sampling time.  */
#define ADC_SPT2_CSPT5(n)       ((n) << 15) /**< @brief AN5 sampling time.  */
#define ADC_SPT2_CSPT6(n)       ((n) << 18) /**< @brief AN6 sampling time.  */
#define ADC_SPT2_CSPT7(n)       ((n) << 21) /**< @brief AN7 sampling time.  */
#define ADC_SPT2_CSPT8(n)       ((n) << 24) /**< @brief AN8 sampling time.  */
#define ADC_SPT2_CSPT9(n)       ((n) << 27) /**< @brief AN9 sampling time.  */

#define ADC_SPT1_CSPT10(n)      ((n) << 0)  /**< @brief AN10 sampling time. */
#define ADC_SPT1_CSPT11(n)      ((n) << 3)  /**< @brief AN11 sampling time. */
#define ADC_SPT1_CSPT12(n)      ((n) << 6)  /**< @brief AN12 sampling time. */
#define ADC_SPT1_CSPT13(n)      ((n) << 9)  /**< @brief AN13 sampling time. */
#define ADC_SPT1_CSPT14(n)      ((n) << 12) /**< @brief AN14 sampling time. */
#define ADC_SPT1_CSPT15(n)      ((n) << 15) /**< @brief AN15 sampling time. */
#define ADC_SPT1_CSPT16(n)      ((n) << 18) /**< @brief Temperature Sensor
                                                 sampling time.             */
#define ADC_SPT1_CSPT17(n)      ((n) << 21) /**< @brief Voltage Reference
                                                 sampling time.             */
/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if AT32_ADC_USE_ADC1 && !defined(__DOXYGEN__)
extern ADCDriver ADCD1;
#endif

#if AT32_ADC_USE_ADC2 && !defined(__DOXYGEN__)
extern ADCDriver ADCD2;
#endif

#if AT32_ADC_USE_ADC3 && !defined(__DOXYGEN__)
extern ADCDriver ADCD3;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void adc_lld_init(void);
  void adc_lld_start(ADCDriver *adcp);
  void adc_lld_stop(ADCDriver *adcp);
  void adc_lld_start_conversion(ADCDriver *adcp);
  void adc_lld_stop_conversion(ADCDriver *adcp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_ADC */

#endif /* HAL_ADC_LLD_H */

/** @} */
