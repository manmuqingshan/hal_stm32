/**
  ******************************************************************************
  * @file    stm32wbxx_ll_adc.c
  * @author  MCD Application Team
  * @brief   ADC LL module driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_ll_adc.h"
#include "stm32wbxx_ll_bus.h"

#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif

/** @addtogroup STM32WBxx_LL_Driver
  * @{
  */

#if defined (ADC1)

/** @addtogroup ADC_LL ADC
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @addtogroup ADC_LL_Private_Constants
  * @{
  */

/* Definitions of ADC hardware constraints delays */
/* Note: Only ADC peripheral HW delays are defined in ADC LL driver driver,   */
/*       not timeout values:                                                  */
/*       Timeout values for ADC operations are dependent to device clock      */
/*       configuration (system clock versus ADC clock),                       */
/*       and therefore must be defined in user application.                   */
/*       Refer to @ref ADC_LL_EC_HW_DELAYS for description of ADC timeout     */
/*       values definition.                                                   */
/* Note: ADC timeout values are defined here in CPU cycles to be independent  */
/*       of device clock setting.                                             */
/*       In user application, ADC timeout values should be defined with       */
/*       temporal values, in function of device clock settings.               */
/*       Highest ratio CPU clock frequency vs ADC clock frequency:            */
/*        - ADC clock from synchronous clock with AHB prescaler 512,          */
/*          APB prescaler 16, ADC prescaler 4.                                */
/*        - ADC clock from asynchronous clock (PLLSAI) with prescaler 1,      */
/*          with highest ratio CPU clock frequency vs HSI clock frequency:    */
/*          CPU clock frequency max 72MHz, PLLSAI freq min 26MHz: ratio 4.    */
/* Unit: CPU cycles.                                                          */
#define ADC_CLOCK_RATIO_VS_CPU_HIGHEST          (512UL * 16UL * 4UL)
#define ADC_TIMEOUT_DISABLE_CPU_CYCLES          (ADC_CLOCK_RATIO_VS_CPU_HIGHEST * 1UL)
#define ADC_TIMEOUT_STOP_CONVERSION_CPU_CYCLES  (ADC_CLOCK_RATIO_VS_CPU_HIGHEST * 1UL)
#if defined(ADC_SUPPORT_2_5_MSPS)
/* Note: CCRDY handshake requires 1APB + 2 ADC + 3 APB cycles                 */
/*       after the channel configuration has been changed.                    */
/*       Driver timeout is approximated to 6 CPU cycles.                      */
#define ADC_TIMEOUT_CCRDY_CPU_CYCLES            (ADC_CLOCK_RATIO_VS_CPU_HIGHEST * 6UL)
#endif /* ADC_SUPPORT_2_5_MSPS */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/** @addtogroup ADC_LL_Private_Macros
  * @{
  */

/* Check of parameters for configuration of ADC hierarchical scope:           */
/* common to several ADC instances.                                           */
#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_COMMON_CLOCK(__CLOCK__)                                      \
  (   ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV1)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV2)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV4)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV6)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV8)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV10)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV12)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV16)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV32)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV64)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV128)                               \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV256)                               \
  )
#else
#define IS_LL_ADC_COMMON_CLOCK(__CLOCK__)                                      \
  (   ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV1)                             \
   || ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV2)                             \
   || ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV4)                             \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV1)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV2)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV4)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV6)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV8)                                 \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV10)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV12)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV16)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV32)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV64)                                \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV128)                               \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV256)                               \
  )
#endif /* ADC_SUPPORT_2_5_MSPS */

/* Check of parameters for configuration of ADC hierarchical scope:           */
/* ADC instance.                                                              */
#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_CLOCK(__CLOCK__)                                             \
  (   ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV4)                             \
   || ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV2)                             \
   || ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV1)                             \
   || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC)                                      \
  )

#endif /* ADC_SUPPORT_2_5_MSPS */
#define IS_LL_ADC_RESOLUTION(__RESOLUTION__)                                   \
  (   ((__RESOLUTION__) == LL_ADC_RESOLUTION_12B)                              \
   || ((__RESOLUTION__) == LL_ADC_RESOLUTION_10B)                              \
   || ((__RESOLUTION__) == LL_ADC_RESOLUTION_8B)                               \
   || ((__RESOLUTION__) == LL_ADC_RESOLUTION_6B)                               \
  )

#define IS_LL_ADC_DATA_ALIGN(__DATA_ALIGN__)                                   \
  (   ((__DATA_ALIGN__) == LL_ADC_DATA_ALIGN_RIGHT)                            \
   || ((__DATA_ALIGN__) == LL_ADC_DATA_ALIGN_LEFT)                             \
  )

#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_LOW_POWER(__LOW_POWER__)                                     \
  (   ((__LOW_POWER__) == LL_ADC_LP_MODE_NONE)                                 \
   || ((__LOW_POWER__) == LL_ADC_LP_AUTOWAIT)                                  \
   || ((__LOW_POWER__) == LL_ADC_LP_AUTOPOWEROFF)                              \
   || ((__LOW_POWER__) == LL_ADC_LP_AUTOWAIT_AUTOPOWEROFF)                     \
  )
#else
#define IS_LL_ADC_LOW_POWER(__LOW_POWER__)                                     \
  (   ((__LOW_POWER__) == LL_ADC_LP_MODE_NONE)                                 \
   || ((__LOW_POWER__) == LL_ADC_LP_AUTOWAIT)                                  \
  )
#endif /* ADC_SUPPORT_2_5_MSPS */
/* Check of parameters for configuration of ADC hierarchical scope:           */
/* ADC group regular                                                          */
#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_REG_TRIG_SOURCE(__REG_TRIG_SOURCE__)                         \
  (   ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_SOFTWARE)                      \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_TRGO2)                \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH4 )                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_CH4)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_CH3)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_EXTI_LINE11)               \
  )
#else
#define IS_LL_ADC_REG_TRIG_SOURCE(__REG_TRIG_SOURCE__)                         \
  (   ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_SOFTWARE)                      \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_TRGO2)                \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH1)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH2)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH3)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_CH2)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_EXTI_LINE11)               \
  )
#endif /* ADC_SUPPORT_2_5_MSPS */

#define IS_LL_ADC_REG_CONTINUOUS_MODE(__REG_CONTINUOUS_MODE__)                 \
  (   ((__REG_CONTINUOUS_MODE__) == LL_ADC_REG_CONV_SINGLE)                    \
   || ((__REG_CONTINUOUS_MODE__) == LL_ADC_REG_CONV_CONTINUOUS)                \
  )

#define IS_LL_ADC_REG_DMA_TRANSFER(__REG_DMA_TRANSFER__)                       \
  (   ((__REG_DMA_TRANSFER__) == LL_ADC_REG_DMA_TRANSFER_NONE)                 \
   || ((__REG_DMA_TRANSFER__) == LL_ADC_REG_DMA_TRANSFER_LIMITED)              \
   || ((__REG_DMA_TRANSFER__) == LL_ADC_REG_DMA_TRANSFER_UNLIMITED)            \
  )

#define IS_LL_ADC_REG_OVR_DATA_BEHAVIOR(__REG_OVR_DATA_BEHAVIOR__)             \
  (   ((__REG_OVR_DATA_BEHAVIOR__) == LL_ADC_REG_OVR_DATA_PRESERVED)           \
   || ((__REG_OVR_DATA_BEHAVIOR__) == LL_ADC_REG_OVR_DATA_OVERWRITTEN)         \
  )

#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_REG_SEQ_MODE(__REG_SEQ_MODE__)                               \
  (   ((__REG_SEQ_MODE__) == LL_ADC_REG_SEQ_FIXED)                             \
   || ((__REG_SEQ_MODE__) == LL_ADC_REG_SEQ_CONFIGURABLE)                      \
  )
#endif /* ADC_SUPPORT_2_5_MSPS */

#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_REG_SEQ_SCAN_LENGTH(__REG_SEQ_SCAN_LENGTH__)                 \
  (   ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_DISABLE)               \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS)         \
  )
#else
#define IS_LL_ADC_REG_SEQ_SCAN_LENGTH(__REG_SEQ_SCAN_LENGTH__)                 \
  (   ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_DISABLE)               \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS)        \
  )
#endif /* ADC_SUPPORT_2_5_MSPS */
#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(__REG_SEQ_DISCONT_MODE__)          \
  (   ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_DISABLE)           \
   || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_1RANK)             \
  )
#else
#define IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(__REG_SEQ_DISCONT_MODE__)          \
  (   ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_DISABLE)           \
   || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_1RANK)             \
   || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_2RANKS)            \
   || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_3RANKS)            \
   || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_4RANKS)            \
   || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_5RANKS)            \
   || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_6RANKS)            \
   || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_7RANKS)            \
   || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_8RANKS)            \
  )
#endif /* ADC_SUPPORT_2_5_MSPS */
/* Check of parameters for configuration of ADC hierarchical scope:           */
/* ADC group injected                                                         */
#define IS_LL_ADC_INJ_TRIG_SOURCE(__INJ_TRIG_SOURCE__)                         \
  (   ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_SOFTWARE)                      \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM1_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM1_TRGO2)                \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM1_CH4)                  \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM2_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM2_CH1)                  \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_EXTI_LINE15)               \
  )

#define IS_LL_ADC_INJ_TRIG_EXT_EDGE(__INJ_TRIG_EXT_EDGE__)                     \
  (   ((__INJ_TRIG_EXT_EDGE__) == LL_ADC_INJ_TRIG_EXT_RISING)                  \
   || ((__INJ_TRIG_EXT_EDGE__) == LL_ADC_INJ_TRIG_EXT_FALLING)                 \
   || ((__INJ_TRIG_EXT_EDGE__) == LL_ADC_INJ_TRIG_EXT_RISINGFALLING)           \
  )

#define IS_LL_ADC_INJ_TRIG_AUTO(__INJ_TRIG_AUTO__)                             \
  (   ((__INJ_TRIG_AUTO__) == LL_ADC_INJ_TRIG_INDEPENDENT)                     \
   || ((__INJ_TRIG_AUTO__) == LL_ADC_INJ_TRIG_FROM_GRP_REGULAR)                \
  )

#define IS_LL_ADC_INJ_SEQ_SCAN_LENGTH(__INJ_SEQ_SCAN_LENGTH__)                 \
  (   ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_DISABLE)               \
   || ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS)         \
   || ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS)         \
   || ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS)         \
  )

#define IS_LL_ADC_INJ_SEQ_SCAN_DISCONT_MODE(__INJ_SEQ_DISCONT_MODE__)          \
  (   ((__INJ_SEQ_DISCONT_MODE__) == LL_ADC_INJ_SEQ_DISCONT_DISABLE)           \
   || ((__INJ_SEQ_DISCONT_MODE__) == LL_ADC_INJ_SEQ_DISCONT_1RANK)             \
  )

/**
  * @}
  */


/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup ADC_LL_Exported_Functions
  * @{
  */

/** @addtogroup ADC_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize registers of all ADC instances belonging to
  *         the same ADC common instance to their default reset values.
  * @note   This function is performing a hard reset, using high level
  *         clock source RCC ADC reset.
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC common registers are de-initialized
  *          - ERROR: not applicable
  */
ErrorStatus LL_ADC_CommonDeInit(ADC_Common_TypeDef *ADCxy_COMMON)
{
  /* Check the parameters */
  assert_param(IS_ADC_COMMON_INSTANCE(ADCxy_COMMON));

  /* Prevent unused argument compilation warning */
  (void)(ADCxy_COMMON);

#if defined(ADC_SUPPORT_2_5_MSPS)
  /* Force reset of ADC clock (core clock) */
  LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_ADC);

  /* Release reset of ADC clock (core clock) */
  LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_ADC);
#else
  /* Force reset of ADC clock (core clock) */
  LL_AHB2_GRP1_ForceReset(LL_AHB2_GRP1_PERIPH_ADC);

  /* Release reset of ADC clock (core clock) */
  LL_AHB2_GRP1_ReleaseReset(LL_AHB2_GRP1_PERIPH_ADC);
#endif /* ADC_SUPPORT_2_5_MSPS */

  return SUCCESS;
}

/**
  * @brief  Initialize some features of ADC common parameters
  *         (all ADC instances belonging to the same ADC common instance)
  *         and multimode (for devices with several ADC instances available).
  * @note   The setting of ADC common parameters is conditioned to
  *         ADC instances state:
  *         All ADC instances belonging to the same ADC common instance
  *         must be disabled.
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @param  ADC_CommonInitStruct Pointer to a @ref LL_ADC_CommonInitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC common registers are initialized
  *          - ERROR: ADC common registers are not initialized
  */
ErrorStatus LL_ADC_CommonInit(ADC_Common_TypeDef *ADCxy_COMMON, const LL_ADC_CommonInitTypeDef *ADC_CommonInitStruct)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_ADC_COMMON_INSTANCE(ADCxy_COMMON));
  assert_param(IS_LL_ADC_COMMON_CLOCK(ADC_CommonInitStruct->CommonClock));

  /* Note: Hardware constraint (refer to description of functions             */
  /*       "LL_ADC_SetCommonXXX()":                                           */
  /*       On this STM32 series, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       All ADC instances of the ADC common group must be disabled.        */
  if (__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(ADCxy_COMMON) == 0UL)
  {
    /* Configuration of ADC hierarchical scope:                               */
    /*  - common to several ADC                                               */
    /*    (all ADC instances belonging to the same ADC common instance)       */
    /*    - Set ADC clock (conversion clock)                                  */
#if defined(ADC_MULTIMODE_SUPPORT)
    if (ADC_CommonInitStruct->Multimode != LL_ADC_MULTI_INDEPENDENT)
    {
      MODIFY_REG(ADCxy_COMMON->CCR,
                 ADC_CCR_CKMODE
                 | ADC_CCR_PRESC
                 | ADC_CCR_DUAL
                 | ADC_CCR_MDMA
                 | ADC_CCR_DELAY
                 ,
                 ADC_CommonInitStruct->CommonClock
                 | ADC_CommonInitStruct->Multimode
                 | ADC_CommonInitStruct->MultiDMATransfer
                 | ADC_CommonInitStruct->MultiTwoSamplingDelay
                );
    }
    else
    {
      MODIFY_REG(ADCxy_COMMON->CCR,
                 ADC_CCR_CKMODE
                 | ADC_CCR_PRESC
                 | ADC_CCR_DUAL
                 | ADC_CCR_MDMA
                 | ADC_CCR_DELAY
                 ,
                 ADC_CommonInitStruct->CommonClock
                 | LL_ADC_MULTI_INDEPENDENT
                );
    }
#else
    LL_ADC_SetCommonClock(ADCxy_COMMON, ADC_CommonInitStruct->CommonClock);
#endif
  }
  else
  {
    /* Initialization error: One or several ADC instances belonging to        */
    /* the same ADC common instance are not disabled.                         */
    status = ERROR;
  }

  return status;
}

/**
  * @brief  Set each @ref LL_ADC_CommonInitTypeDef field to default value.
  * @param  ADC_CommonInitStruct Pointer to a @ref LL_ADC_CommonInitTypeDef structure
  *                              whose fields will be set to default values.
  * @retval None
  */
void LL_ADC_CommonStructInit(LL_ADC_CommonInitTypeDef *ADC_CommonInitStruct)
{
  /* Set ADC_CommonInitStruct fields to default values */
  /* Set fields of ADC common */
  /* (all ADC instances belonging to the same ADC common instance) */
#if defined(ADC_SUPPORT_2_5_MSPS)
  ADC_CommonInitStruct->CommonClock = LL_ADC_CLOCK_ASYNC_DIV2;
#else
  ADC_CommonInitStruct->CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
#endif /* ADC_SUPPORT_2_5_MSPS */

}

/**
  * @brief  De-initialize registers of the selected ADC instance
  *         to their default reset values.
  * @note   To reset all ADC instances quickly (perform a hard reset),
  *         use function @ref LL_ADC_CommonDeInit().
  * @note   If this functions returns error status, it means that ADC instance
  *         is in an unknown state.
  *         In this case, perform a hard reset using high level
  *         clock source RCC ADC reset.
  *         Refer to function @ref LL_ADC_CommonDeInit().
  * @param  ADCx ADC instance
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC registers are de-initialized
  *          - ERROR: ADC registers are not de-initialized
  */
ErrorStatus LL_ADC_DeInit(ADC_TypeDef *ADCx)
{
  ErrorStatus status = SUCCESS;

  __IO uint32_t timeout_cpu_cycles = 0UL;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(ADCx));

  /* Disable ADC instance if not already disabled.                            */
  if (LL_ADC_IsEnabled(ADCx) == 1UL)
  {
    /* Set ADC group regular trigger source to SW start to ensure to not      */
    /* have an external trigger event occurring during the conversion stop    */
    /* ADC disable process.                                                   */
    LL_ADC_REG_SetTriggerSource(ADCx, LL_ADC_REG_TRIG_SOFTWARE);

    /* Stop potential ADC conversion on going on ADC group regular.           */
    if (LL_ADC_REG_IsConversionOngoing(ADCx) != 0UL)
    {
      if (LL_ADC_REG_IsStopConversionOngoing(ADCx) == 0UL)
      {
        LL_ADC_REG_StopConversion(ADCx);
      }
    }

#if !defined(ADC_SUPPORT_2_5_MSPS)
    /* Set ADC group injected trigger source to SW start to ensure to not     */
    /* have an external trigger event occurring during the conversion stop    */
    /* ADC disable process.                                                   */
    LL_ADC_INJ_SetTriggerSource(ADCx, LL_ADC_INJ_TRIG_SOFTWARE);

    /* Stop potential ADC conversion on going on ADC group injected.          */
    if (LL_ADC_INJ_IsConversionOngoing(ADCx) != 0UL)
    {
      if (LL_ADC_INJ_IsStopConversionOngoing(ADCx) == 0UL)
      {
        LL_ADC_INJ_StopConversion(ADCx);
      }
    }

    /* Wait for ADC conversions are effectively stopped                       */
    timeout_cpu_cycles = ADC_TIMEOUT_STOP_CONVERSION_CPU_CYCLES;
    while ((LL_ADC_REG_IsStopConversionOngoing(ADCx)
            | LL_ADC_INJ_IsStopConversionOngoing(ADCx)) == 1UL)
    {
      timeout_cpu_cycles--;
      if (timeout_cpu_cycles == 0UL)
      {
        /* Time-out error */
        status = ERROR;
        break;
      }
    }
#else
    /* Wait for ADC conversions are effectively stopped                       */
    timeout_cpu_cycles = ADC_TIMEOUT_STOP_CONVERSION_CPU_CYCLES;
    while (LL_ADC_REG_IsStopConversionOngoing(ADCx) == 1UL)
    {
      timeout_cpu_cycles--;
      if(timeout_cpu_cycles == 0UL)
      {
        /* Time-out error */
        status = ERROR;
        break;
      }
    }
#endif /* !ADC_SUPPORT_2_5_MSPS */

#if !defined(ADC_SUPPORT_2_5_MSPS)
    /* Flush group injected contexts queue (register JSQR):                   */
    /* Note: Bit JQM must be set to empty the contexts queue (otherwise       */
    /*       contexts queue is maintained with the last active context).      */
    LL_ADC_INJ_SetQueueMode(ADCx, LL_ADC_INJ_QUEUE_2CONTEXTS_END_EMPTY);

#endif /* !ADC_SUPPORT_2_5_MSPS */
    /* Disable the ADC instance */
    LL_ADC_Disable(ADCx);

    /* Wait for ADC instance is effectively disabled */
    timeout_cpu_cycles = ADC_TIMEOUT_DISABLE_CPU_CYCLES;
    while (LL_ADC_IsDisableOngoing(ADCx) == 1UL)
    {
      timeout_cpu_cycles--;
      if (timeout_cpu_cycles == 0UL)
      {
        /* Time-out error */
        status = ERROR;
        break;
      }
    }
  }

  /* Check whether ADC state is compliant with expected state */
#if defined(ADC_SUPPORT_2_5_MSPS)
  if (READ_BIT(ADCx->CR,
               (ADC_CR_ADSTP | ADC_CR_ADSTART
                | ADC_CR_ADDIS | ADC_CR_ADEN   )
              )
      == 0UL)
#else
  if (READ_BIT(ADCx->CR,
               (ADC_CR_JADSTP | ADC_CR_ADSTP | ADC_CR_JADSTART | ADC_CR_ADSTART
                | ADC_CR_ADDIS | ADC_CR_ADEN)
              )
      == 0UL)
#endif /* ADC_SUPPORT_2_5_MSPS */
  {
    /* ========== Reset ADC registers ========== */
    /* Reset register IER */
#if !defined(ADC_SUPPORT_2_5_MSPS)
    CLEAR_BIT(ADCx->IER,
              (LL_ADC_IT_ADRDY
               | LL_ADC_IT_EOC
               | LL_ADC_IT_EOS
               | LL_ADC_IT_OVR
               | LL_ADC_IT_EOSMP
               | LL_ADC_IT_JEOC
               | LL_ADC_IT_JEOS
               | LL_ADC_IT_JQOVF
               | LL_ADC_IT_AWD1
               | LL_ADC_IT_AWD2
               | LL_ADC_IT_AWD3
              )
             );
#endif /* !ADC_SUPPORT_2_5_MSPS */
#if defined(ADC_SUPPORT_2_5_MSPS)
    CLEAR_BIT(ADCx->IER,
              (LL_ADC_IT_ADRDY
               | LL_ADC_IT_EOC
               | LL_ADC_IT_EOS
               | LL_ADC_IT_OVR
               | LL_ADC_IT_EOSMP
               | LL_ADC_IT_CCRDY
               | LL_ADC_IT_AWD1
               | LL_ADC_IT_AWD2
               | LL_ADC_IT_AWD3
              )
             );
#endif /* ADC_SUPPORT_2_5_MSPS */

    /* Reset register ISR */
#if !defined(ADC_SUPPORT_2_5_MSPS)
    SET_BIT(ADCx->ISR,
            (LL_ADC_FLAG_ADRDY
             | LL_ADC_FLAG_EOC
             | LL_ADC_FLAG_EOS
             | LL_ADC_FLAG_OVR
             | LL_ADC_FLAG_EOSMP
             | LL_ADC_FLAG_JEOC
             | LL_ADC_FLAG_JEOS
             | LL_ADC_FLAG_JQOVF
             | LL_ADC_FLAG_AWD1
             | LL_ADC_FLAG_AWD2
             | LL_ADC_FLAG_AWD3
            )
           );
#endif /* !ADC_SUPPORT_2_5_MSPS */
#if defined(ADC_SUPPORT_2_5_MSPS)
    SET_BIT(ADCx->ISR,
            (LL_ADC_FLAG_ADRDY
             | LL_ADC_FLAG_EOC
             | LL_ADC_FLAG_EOS
             | LL_ADC_FLAG_OVR
             | LL_ADC_FLAG_EOSMP
             | LL_ADC_FLAG_CCRDY
             | LL_ADC_FLAG_AWD1
             | LL_ADC_FLAG_AWD2
             | LL_ADC_FLAG_AWD3
            )
           );
#endif /* ADC_SUPPORT_2_5_MSPS */

#if defined(ADC_SUPPORT_2_5_MSPS)
    /* Reset register CR */
    /* Bits ADC_CR_ADCAL, ADC_CR_ADSTP, ADC_CR_ADSTART are in access mode     */
    /* "read-set": no direct reset applicable.                                */
    CLEAR_BIT(ADCx->CR, ADC_CR_ADVREGEN);
#else
    /* Reset register CR */
    /*  - Bits ADC_CR_JADSTP, ADC_CR_ADSTP, ADC_CR_JADSTART, ADC_CR_ADSTART,  */
    /*    ADC_CR_ADCAL, ADC_CR_ADDIS, ADC_CR_ADEN are in                      */
    /*    access mode "read-set": no direct reset applicable.                 */
    /*  - Reset Calibration mode to default setting (single ended).           */
    /*  - Disable ADC internal voltage regulator.                             */
    /*  - Enable ADC deep power down.                                         */
    /*    Note: ADC internal voltage regulator disable and ADC deep power     */
    /*          down enable are conditioned to ADC state disabled:            */
    /*          already done above.                                           */
    CLEAR_BIT(ADCx->CR, ADC_CR_ADVREGEN | ADC_CR_ADCALDIF);
    SET_BIT(ADCx->CR, ADC_CR_DEEPPWD);
#endif /* ADC_SUPPORT_2_5_MSPS */

#if defined(ADC_SUPPORT_2_5_MSPS)
    /* Reset register CFGR1 */
    CLEAR_BIT(ADCx->CFGR1,
              (  ADC_CFGR1_AWD1CH  | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL | ADC_CFGR1_DISCEN
               | ADC_CFGR1_AUTOFF  | ADC_CFGR1_WAIT   | ADC_CFGR1_CONT    | ADC_CFGR1_OVRMOD
               | ADC_CFGR1_EXTEN   | ADC_CFGR1_EXTSEL | ADC_CFGR1_ALIGN   | ADC_CFGR1_RES
               | ADC_CFGR1_SCANDIR | ADC_CFGR1_DMACFG | ADC_CFGR1_DMAEN                     )
             );

    /* Reset register SMPR */
    CLEAR_BIT(ADCx->SMPR, ADC_SMPR_SMP1 | ADC_SMPR_SMP2 | ADC_SMPR_SMPSEL);

    /* Reset register TR */
    MODIFY_REG(ADCx->TR, ADC_TR_HT | ADC_TR_LT, ADC_TR_HT);

    /* Reset register CHSELR */
    CLEAR_BIT(ADCx->CHSELR,
              (  ADC_CHSELR_CHSEL18 | ADC_CHSELR_CHSEL17 | ADC_CHSELR_CHSEL16
               | ADC_CHSELR_CHSEL15 | ADC_CHSELR_CHSEL14 | ADC_CHSELR_CHSEL13 | ADC_CHSELR_CHSEL12
               | ADC_CHSELR_CHSEL11 | ADC_CHSELR_CHSEL10 | ADC_CHSELR_CHSEL9  | ADC_CHSELR_CHSEL8
               | ADC_CHSELR_CHSEL7  | ADC_CHSELR_CHSEL6  | ADC_CHSELR_CHSEL5  | ADC_CHSELR_CHSEL4
               | ADC_CHSELR_CHSEL3  | ADC_CHSELR_CHSEL2  | ADC_CHSELR_CHSEL1  | ADC_CHSELR_CHSEL0 )
             );

    /* Wait for ADC channel configuration ready */
    timeout_cpu_cycles = ADC_TIMEOUT_CCRDY_CPU_CYCLES;
    while (LL_ADC_IsActiveFlag_CCRDY(ADCx) == 0UL)
    {
      timeout_cpu_cycles--;
      if(timeout_cpu_cycles == 0UL)
      {
        /* Time-out error */
        status = ERROR;
        break;
      }
    }

    /* Clear flag ADC channel configuration ready */
    LL_ADC_ClearFlag_CCRDY(ADCx);

    /* Reset register DR */
    /* bits in access mode read only, no direct reset applicable */

    /* Reset register CALFACT */
    CLEAR_BIT(ADCx->CALFACT, ADC_CALFACT_CALFACT);

    /* Reset register CFGR2 */
    /* Note: Update of ADC clock mode is conditioned to ADC state disabled:   */
    /*       already done above.                                              */
    /* Note: Register reset last due to selection of asynchronous clock,      */
    /*       ADC clock then depends of configuration clock source at system   */
    /*       level.                                                           */
    CLEAR_BIT(ADCx->CFGR2,
              (  ADC_CFGR2_CKMODE
               | ADC_CFGR2_LFTRIG                                     )
             );

#else
    /* Reset register CFGR */
    MODIFY_REG(ADCx->CFGR,
               (ADC_CFGR_AWD1CH  | ADC_CFGR_JAUTO   | ADC_CFGR_JAWD1EN
                | ADC_CFGR_AWD1EN  | ADC_CFGR_AWD1SGL | ADC_CFGR_JQM
                | ADC_CFGR_JDISCEN | ADC_CFGR_DISCNUM | ADC_CFGR_DISCEN
                | ADC_CFGR_AUTDLY  | ADC_CFGR_CONT    | ADC_CFGR_OVRMOD
                | ADC_CFGR_EXTEN   | ADC_CFGR_EXTSEL  | ADC_CFGR_ALIGN
                | ADC_CFGR_RES     | ADC_CFGR_DMACFG  | ADC_CFGR_DMAEN),
               ADC_CFGR_JQDIS
              );

    /* Reset register CFGR2 */
    CLEAR_BIT(ADCx->CFGR2,
              (ADC_CFGR2_ROVSM  | ADC_CFGR2_TROVS | ADC_CFGR2_OVSS
               | ADC_CFGR2_OVSR   | ADC_CFGR2_JOVSE | ADC_CFGR2_ROVSE)
             );


    /* Reset register SMPR1 */
    CLEAR_BIT(ADCx->SMPR1,
              (ADC_SMPR1_SMP9 | ADC_SMPR1_SMP8 | ADC_SMPR1_SMP7
               | ADC_SMPR1_SMP6 | ADC_SMPR1_SMP5 | ADC_SMPR1_SMP4
               | ADC_SMPR1_SMP3 | ADC_SMPR1_SMP2 | ADC_SMPR1_SMP1)
             );

    /* Reset register SMPR2 */
    CLEAR_BIT(ADCx->SMPR2,
              (ADC_SMPR2_SMP18 | ADC_SMPR2_SMP17 | ADC_SMPR2_SMP16
               | ADC_SMPR2_SMP15 | ADC_SMPR2_SMP14 | ADC_SMPR2_SMP13
               | ADC_SMPR2_SMP12 | ADC_SMPR2_SMP11 | ADC_SMPR2_SMP10)
             );

    /* Reset register TR1 */
    MODIFY_REG(ADCx->TR1, ADC_TR1_HT1 | ADC_TR1_LT1, ADC_TR1_HT1);

    /* Reset register TR2 */
    MODIFY_REG(ADCx->TR2, ADC_TR2_HT2 | ADC_TR2_LT2, ADC_TR2_HT2);

    /* Reset register TR3 */
    MODIFY_REG(ADCx->TR3, ADC_TR3_HT3 | ADC_TR3_LT3, ADC_TR3_HT3);

    /* Reset register SQR1 */
    CLEAR_BIT(ADCx->SQR1,
              (ADC_SQR1_SQ4 | ADC_SQR1_SQ3 | ADC_SQR1_SQ2
               | ADC_SQR1_SQ1 | ADC_SQR1_L)
             );

    /* Reset register SQR2 */
    CLEAR_BIT(ADCx->SQR2,
              (ADC_SQR2_SQ9 | ADC_SQR2_SQ8 | ADC_SQR2_SQ7
               | ADC_SQR2_SQ6 | ADC_SQR2_SQ5)
             );

    /* Reset register SQR3 */
    CLEAR_BIT(ADCx->SQR3,
              (ADC_SQR3_SQ14 | ADC_SQR3_SQ13 | ADC_SQR3_SQ12
               | ADC_SQR3_SQ11 | ADC_SQR3_SQ10)
             );

    /* Reset register SQR4 */
    CLEAR_BIT(ADCx->SQR4, ADC_SQR4_SQ16 | ADC_SQR4_SQ15);

    /* Reset register JSQR */
    CLEAR_BIT(ADCx->JSQR,
              (ADC_JSQR_JL
               | ADC_JSQR_JEXTSEL | ADC_JSQR_JEXTEN
               | ADC_JSQR_JSQ4    | ADC_JSQR_JSQ3
               | ADC_JSQR_JSQ2    | ADC_JSQR_JSQ1)
             );

    /* Reset register DR */
    /* Note: bits in access mode read only, no direct reset applicable */

    /* Reset register OFR1 */
    CLEAR_BIT(ADCx->OFR1, ADC_OFR1_OFFSET1_EN | ADC_OFR1_OFFSET1_CH | ADC_OFR1_OFFSET1);
    /* Reset register OFR2 */
    CLEAR_BIT(ADCx->OFR2, ADC_OFR2_OFFSET2_EN | ADC_OFR2_OFFSET2_CH | ADC_OFR2_OFFSET2);
    /* Reset register OFR3 */
    CLEAR_BIT(ADCx->OFR3, ADC_OFR3_OFFSET3_EN | ADC_OFR3_OFFSET3_CH | ADC_OFR3_OFFSET3);
    /* Reset register OFR4 */
    CLEAR_BIT(ADCx->OFR4, ADC_OFR4_OFFSET4_EN | ADC_OFR4_OFFSET4_CH | ADC_OFR4_OFFSET4);

    /* Reset registers JDR1, JDR2, JDR3, JDR4 */
    /* Note: bits in access mode read only, no direct reset applicable */

    /* Reset register AWD2CR */
    CLEAR_BIT(ADCx->AWD2CR, ADC_AWD2CR_AWD2CH);

    /* Reset register AWD3CR */
    CLEAR_BIT(ADCx->AWD3CR, ADC_AWD3CR_AWD3CH);

    /* Reset register DIFSEL */
    CLEAR_BIT(ADCx->DIFSEL, ADC_DIFSEL_DIFSEL);

    /* Reset register CALFACT */
    CLEAR_BIT(ADCx->CALFACT, ADC_CALFACT_CALFACT_D | ADC_CALFACT_CALFACT_S);
#endif /* ADC_SUPPORT_2_5_MSPS */
  }
  else
  {
    /* ADC instance is in an unknown state */
    /* Need to performing a hard reset of ADC instance, using high level      */
    /* clock source RCC ADC reset.                                            */
    /* Caution: On this STM32 series, if several ADC instances are available   */
    /*          on the selected device, RCC ADC reset will reset              */
    /*          all ADC instances belonging to the common ADC instance.       */
    status = ERROR;
  }

  return status;
}

/**
  * @brief  Initialize some features of ADC instance.
  * @note   These parameters have an impact on ADC scope: ADC instance.
  *         Affects both group regular and group injected (availability
  *         of ADC group injected depends on STM32 families).
  *         Refer to corresponding unitary functions into
  *         @ref ADC_LL_EF_Configuration_ADC_Instance .
  * @note   The setting of these parameters by function @ref LL_ADC_Init()
  *         is conditioned to ADC state:
  *         ADC instance must be disabled.
  *         This condition is applied to all ADC features, for efficiency
  *         and compatibility over all STM32 families. However, the different
  *         features can be set under different ADC state conditions
  *         (setting possible with ADC enabled without conversion on going,
  *         ADC enabled with conversion on going, ...)
  *         Each feature can be updated afterwards with a unitary function
  *         and potentially with ADC in a different state than disabled,
  *         refer to description of each function for setting
  *         conditioned to ADC state.
  * @note   After using this function, some other features must be configured
  *         using LL unitary functions.
  *         The minimum configuration remaining to be done is:
  *          - Set ADC group regular or group injected sequencer:
  *            map channel on the selected sequencer rank.
  *            Refer to function @ref LL_ADC_REG_SetSequencerRanks().
  *          - Set ADC channel sampling time
  *            Refer to function LL_ADC_SetChannelSamplingTime();
  * @param  ADCx ADC instance
  * @param  ADC_InitStruct Pointer to a @ref LL_ADC_REG_InitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC registers are initialized
  *          - ERROR: ADC registers are not initialized
  */
ErrorStatus LL_ADC_Init(ADC_TypeDef *ADCx, const LL_ADC_InitTypeDef *ADC_InitStruct)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(ADCx));

#if defined(ADC_SUPPORT_2_5_MSPS)
  assert_param(IS_LL_ADC_CLOCK(ADC_InitStruct->Clock));
#endif /* ADC_SUPPORT_2_5_MSPS */
  assert_param(IS_LL_ADC_RESOLUTION(ADC_InitStruct->Resolution));
  assert_param(IS_LL_ADC_DATA_ALIGN(ADC_InitStruct->DataAlignment));
  assert_param(IS_LL_ADC_LOW_POWER(ADC_InitStruct->LowPowerMode));

  /* Note: Hardware constraint (refer to description of this function):       */
  /*       ADC instance must be disabled.                                     */
  if (LL_ADC_IsEnabled(ADCx) == 0UL)
  {
    /* Configuration of ADC hierarchical scope:                               */
    /*  - ADC instance                                                        */
    /*    - Set ADC data resolution                                           */
    /*    - Set ADC conversion data alignment                                 */
    /*    - Set ADC low power mode                                            */
#if defined(ADC_SUPPORT_2_5_MSPS)
    MODIFY_REG(ADCx->CFGR1,
                 ADC_CFGR1_RES
               | ADC_CFGR1_ALIGN
               | ADC_CFGR1_WAIT
               | ADC_CFGR1_AUTOFF
              ,
                 ADC_InitStruct->Resolution
               | ADC_InitStruct->DataAlignment
               | ADC_InitStruct->LowPowerMode
              );

    MODIFY_REG(ADCx->CFGR2,
               ADC_CFGR2_CKMODE
              ,
               ADC_InitStruct->Clock
              );
#else
    MODIFY_REG(ADCx->CFGR,
               ADC_CFGR_RES
               | ADC_CFGR_ALIGN
               | ADC_CFGR_AUTDLY
               ,
               ADC_InitStruct->Resolution
               | ADC_InitStruct->DataAlignment
               | ADC_InitStruct->LowPowerMode
              );
#endif /* ADC_SUPPORT_2_5_MSPS */
  }
  else
  {
    /* Initialization error: ADC instance is not disabled. */
    status = ERROR;
  }
  return status;
}

/**
  * @brief  Set each @ref LL_ADC_InitTypeDef field to default value.
  * @param  ADC_InitStruct Pointer to a @ref LL_ADC_InitTypeDef structure
  *                        whose fields will be set to default values.
  * @retval None
  */
void LL_ADC_StructInit(LL_ADC_InitTypeDef *ADC_InitStruct)
{
  /* Set ADC_InitStruct fields to default values */
  /* Set fields of ADC instance */
#if defined(ADC_SUPPORT_2_5_MSPS)
  ADC_InitStruct->Clock         = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
#endif /* ADC_SUPPORT_2_5_MSPS */
  ADC_InitStruct->Resolution    = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct->DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct->LowPowerMode  = LL_ADC_LP_MODE_NONE;

}

/**
  * @brief  Initialize some features of ADC group regular.
  * @note   These parameters have an impact on ADC scope: ADC group regular.
  *         Refer to corresponding unitary functions into
  *         @ref ADC_LL_EF_Configuration_ADC_Group_Regular
  *         (functions with prefix "REG").
  * @note   The setting of these parameters by function @ref LL_ADC_Init()
  *         is conditioned to ADC state:
  *         ADC instance must be disabled.
  *         This condition is applied to all ADC features, for efficiency
  *         and compatibility over all STM32 families. However, the different
  *         features can be set under different ADC state conditions
  *         (setting possible with ADC enabled without conversion on going,
  *         ADC enabled with conversion on going, ...)
  *         Each feature can be updated afterwards with a unitary function
  *         and potentially with ADC in a different state than disabled,
  *         refer to description of each function for setting
  *         conditioned to ADC state.
  * @note   On devices STM32WB10xx, STM32WB15xx, STM32WB1Mxx: Before using this function,
  *         ADC group regular sequencer must be configured: refer to function
  *         @ref LL_ADC_REG_SetSequencerConfigurable().
  * @note   After using this function, other features must be configured
  *         using LL unitary functions.
  *         The minimum configuration remaining to be done is:
  *          - Set ADC group regular or group injected sequencer:
  *            map channel on the selected sequencer rank.
  *            Refer to function @ref LL_ADC_REG_SetSequencerRanks().
  *          - Set ADC channel sampling time
  *            Refer to function LL_ADC_SetChannelSamplingTime();
  * @param  ADCx ADC instance
  * @param  ADC_REG_InitStruct Pointer to a @ref LL_ADC_REG_InitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC registers are initialized
  *          - ERROR: ADC registers are not initialized
  */
ErrorStatus LL_ADC_REG_Init(ADC_TypeDef *ADCx, const LL_ADC_REG_InitTypeDef *ADC_REG_InitStruct)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(ADCx));
  assert_param(IS_LL_ADC_REG_TRIG_SOURCE(ADC_REG_InitStruct->TriggerSource));
#if defined(ADC_SUPPORT_2_5_MSPS)
  if (LL_ADC_REG_GetSequencerConfigurable(ADCx) != LL_ADC_REG_SEQ_FIXED)
  {
    assert_param(IS_LL_ADC_REG_SEQ_SCAN_LENGTH(ADC_REG_InitStruct->SequencerLength));
  }
  if ((LL_ADC_REG_GetSequencerConfigurable(ADCx) == LL_ADC_REG_SEQ_FIXED)
      || (ADC_REG_InitStruct->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE)
     )
  {
    assert_param(IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(ADC_REG_InitStruct->SequencerDiscont));

    /* ADC group regular continuous mode and discontinuous mode                 */
    /* can not be enabled simultenaeously                                       */
    assert_param((ADC_REG_InitStruct->ContinuousMode == LL_ADC_REG_CONV_SINGLE)
                 || (ADC_REG_InitStruct->SequencerDiscont == LL_ADC_REG_SEQ_DISCONT_DISABLE));
  }
#else
  assert_param(IS_LL_ADC_REG_SEQ_SCAN_LENGTH(ADC_REG_InitStruct->SequencerLength));
  if (ADC_REG_InitStruct->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE)
  {
    assert_param(IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(ADC_REG_InitStruct->SequencerDiscont));

    /* ADC group regular continuous mode and discontinuous mode                 */
    /* can not be enabled simultenaeously                                       */
    assert_param((ADC_REG_InitStruct->ContinuousMode == LL_ADC_REG_CONV_SINGLE)
                 || (ADC_REG_InitStruct->SequencerDiscont == LL_ADC_REG_SEQ_DISCONT_DISABLE));
  }
#endif /* ADC_SUPPORT_2_5_MSPS */
  assert_param(IS_LL_ADC_REG_CONTINUOUS_MODE(ADC_REG_InitStruct->ContinuousMode));
  assert_param(IS_LL_ADC_REG_DMA_TRANSFER(ADC_REG_InitStruct->DMATransfer));
  assert_param(IS_LL_ADC_REG_OVR_DATA_BEHAVIOR(ADC_REG_InitStruct->Overrun));

  /* Note: Hardware constraint (refer to description of this function):       */
  /*       ADC instance must be disabled.                                     */
  if (LL_ADC_IsEnabled(ADCx) == 0UL)
  {
    /* Configuration of ADC hierarchical scope:                               */
    /*  - ADC group regular                                                   */
    /*    - Set ADC group regular trigger source                              */
    /*    - Set ADC group regular sequencer length                            */
    /*    - Set ADC group regular sequencer discontinuous mode                */
    /*    - Set ADC group regular continuous mode                             */
    /*    - Set ADC group regular conversion data transfer: no transfer or    */
    /*      transfer by DMA, and DMA requests mode                            */
    /*    - Set ADC group regular overrun behavior                            */
    /* Note: On this STM32 series, ADC trigger edge is set to value 0x0 by     */
    /*       setting of trigger source to SW start.                           */
#if defined(ADC_SUPPORT_2_5_MSPS)
    if(   (LL_ADC_REG_GetSequencerConfigurable(ADCx) == LL_ADC_REG_SEQ_FIXED)
       || (ADC_REG_InitStruct->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE)
      )
    {
      /* Case of sequencer mode fixed
         or sequencer length >= 2 ranks with sequencer mode fully configurable:
         discontinuous mode configured */
      MODIFY_REG(ADCx->CFGR1,
                   ADC_CFGR1_EXTSEL
                 | ADC_CFGR1_EXTEN
                 | ADC_CFGR1_DISCEN
                 | ADC_CFGR1_CONT
                 | ADC_CFGR1_DMAEN
                 | ADC_CFGR1_DMACFG
                 | ADC_CFGR1_OVRMOD
                ,
                   ADC_REG_InitStruct->TriggerSource
                 | ADC_REG_InitStruct->SequencerDiscont
                 | ADC_REG_InitStruct->ContinuousMode
                 | ADC_REG_InitStruct->DMATransfer
                 | ADC_REG_InitStruct->Overrun
                );
    }
    else
    {
      /* Case of sequencer mode fully configurable
         and sequencer length 1 rank (sequencer disabled):
         discontinuous mode discarded (fixed to disable) */
      MODIFY_REG(ADCx->CFGR1,
                   ADC_CFGR1_EXTSEL
                 | ADC_CFGR1_EXTEN
                 | ADC_CFGR1_DISCEN
                 | ADC_CFGR1_CONT
                 | ADC_CFGR1_DMAEN
                 | ADC_CFGR1_DMACFG
                 | ADC_CFGR1_OVRMOD
                ,
                   ADC_REG_InitStruct->TriggerSource
                 | LL_ADC_REG_SEQ_DISCONT_DISABLE
                 | ADC_REG_InitStruct->ContinuousMode
                 | ADC_REG_InitStruct->DMATransfer
                 | ADC_REG_InitStruct->Overrun
                );
    }
#else
    if (ADC_REG_InitStruct->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE)
    {
      MODIFY_REG(ADCx->CFGR,
                 ADC_CFGR_EXTSEL
                 | ADC_CFGR_EXTEN
                 | ADC_CFGR_DISCEN
                 | ADC_CFGR_DISCNUM
                 | ADC_CFGR_CONT
                 | ADC_CFGR_DMAEN
                 | ADC_CFGR_DMACFG
                 | ADC_CFGR_OVRMOD
                 ,
                 ADC_REG_InitStruct->TriggerSource
                 | ADC_REG_InitStruct->SequencerDiscont
                 | ADC_REG_InitStruct->ContinuousMode
                 | ADC_REG_InitStruct->DMATransfer
                 | ADC_REG_InitStruct->Overrun
                );
    }
    else
    {
      MODIFY_REG(ADCx->CFGR,
                 ADC_CFGR_EXTSEL
                 | ADC_CFGR_EXTEN
                 | ADC_CFGR_DISCEN
                 | ADC_CFGR_DISCNUM
                 | ADC_CFGR_CONT
                 | ADC_CFGR_DMAEN
                 | ADC_CFGR_DMACFG
                 | ADC_CFGR_OVRMOD
                 ,
                 ADC_REG_InitStruct->TriggerSource
                 | LL_ADC_REG_SEQ_DISCONT_DISABLE
                 | ADC_REG_InitStruct->ContinuousMode
                 | ADC_REG_InitStruct->DMATransfer
                 | ADC_REG_InitStruct->Overrun
                );
    }
#endif /* ADC_SUPPORT_2_5_MSPS */

    /* Set ADC group regular sequencer length and scan direction */
#if defined(ADC_SUPPORT_2_5_MSPS)
    if (LL_ADC_REG_GetSequencerConfigurable(ADCx) != LL_ADC_REG_SEQ_FIXED)
    {
      LL_ADC_REG_SetSequencerLength(ADCx, ADC_REG_InitStruct->SequencerLength);
    }
#else
    LL_ADC_REG_SetSequencerLength(ADCx, ADC_REG_InitStruct->SequencerLength);
#endif /* ADC_SUPPORT_2_5_MSPS */
  }
  else
  {
    /* Initialization error: ADC instance is not disabled. */
    status = ERROR;
  }
  return status;
}

/**
  * @brief  Set each @ref LL_ADC_REG_InitTypeDef field to default value.
  * @param  ADC_REG_InitStruct Pointer to a @ref LL_ADC_REG_InitTypeDef structure
  *                            whose fields will be set to default values.
  * @retval None
  */
void LL_ADC_REG_StructInit(LL_ADC_REG_InitTypeDef *ADC_REG_InitStruct)
{
  /* Set ADC_REG_InitStruct fields to default values */
  /* Set fields of ADC group regular */
  /* Note: On this STM32 series, ADC trigger edge is set to value 0x0 by       */
  /*       setting of trigger source to SW start.                             */
  ADC_REG_InitStruct->TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct->SequencerLength  = LL_ADC_REG_SEQ_SCAN_DISABLE;
  ADC_REG_InitStruct->SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct->ContinuousMode   = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct->DMATransfer      = LL_ADC_REG_DMA_TRANSFER_NONE;
  ADC_REG_InitStruct->Overrun          = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
}

#if  defined(ADC_SUPPORT_2_5_MSPS)
/* Feature "ADC group injected" not available on ADC peripheral of this STM32WB device */
#else
/**
  * @brief  Initialize some features of ADC group injected.
  * @note   These parameters have an impact on ADC scope: ADC group injected.
  *         Refer to corresponding unitary functions into
  *         @ref ADC_LL_EF_Configuration_ADC_Group_Regular
  *         (functions with prefix "INJ").
  * @note   The setting of these parameters by function @ref LL_ADC_Init()
  *         is conditioned to ADC state:
  *         ADC instance must be disabled.
  *         This condition is applied to all ADC features, for efficiency
  *         and compatibility over all STM32 families. However, the different
  *         features can be set under different ADC state conditions
  *         (setting possible with ADC enabled without conversion on going,
  *         ADC enabled with conversion on going, ...)
  *         Each feature can be updated afterwards with a unitary function
  *         and potentially with ADC in a different state than disabled,
  *         refer to description of each function for setting
  *         conditioned to ADC state.
  * @note   After using this function, other features must be configured
  *         using LL unitary functions.
  *         The minimum configuration remaining to be done is:
  *          - Set ADC group injected sequencer:
  *            map channel on the selected sequencer rank.
  *            Refer to function @ref LL_ADC_INJ_SetSequencerRanks().
  *          - Set ADC channel sampling time
  *            Refer to function LL_ADC_SetChannelSamplingTime();
  * @note   Caution if feature ADC group injected contexts queue is enabled
  *         (refer to with function @ref LL_ADC_INJ_SetQueueMode() ):
  *         using successively several times this function will appear as
  *         having no effect.
  *         To set several features of ADC group injected, use
  *         function @ref LL_ADC_INJ_ConfigQueueContext().
  * @param  ADCx ADC instance
  * @param  ADC_INJ_InitStruct Pointer to a @ref LL_ADC_INJ_InitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC registers are initialized
  *          - ERROR: ADC registers are not initialized
  */
ErrorStatus LL_ADC_INJ_Init(ADC_TypeDef *ADCx, const LL_ADC_INJ_InitTypeDef *ADC_INJ_InitStruct)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(ADCx));
  assert_param(IS_LL_ADC_INJ_TRIG_SOURCE(ADC_INJ_InitStruct->TriggerSource));
  assert_param(IS_LL_ADC_INJ_SEQ_SCAN_LENGTH(ADC_INJ_InitStruct->SequencerLength));
  if (ADC_INJ_InitStruct->SequencerLength != LL_ADC_INJ_SEQ_SCAN_DISABLE)
  {
    assert_param(IS_LL_ADC_INJ_SEQ_SCAN_DISCONT_MODE(ADC_INJ_InitStruct->SequencerDiscont));
  }
  assert_param(IS_LL_ADC_INJ_TRIG_AUTO(ADC_INJ_InitStruct->TrigAuto));

  /* Note: Hardware constraint (refer to description of this function):       */
  /*       ADC instance must be disabled.                                     */
  if (LL_ADC_IsEnabled(ADCx) == 0UL)
  {
    /* Configuration of ADC hierarchical scope:                               */
    /*  - ADC group injected                                                  */
    /*    - Set ADC group injected trigger source                             */
    /*    - Set ADC group injected sequencer length                           */
    /*    - Set ADC group injected sequencer discontinuous mode               */
    /*    - Set ADC group injected conversion trigger: independent or         */
    /*      from ADC group regular                                            */
    /* Note: On this STM32 series, ADC trigger edge is set to value 0x0 by     */
    /*       setting of trigger source to SW start.                           */
    if (ADC_INJ_InitStruct->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE)
    {
      MODIFY_REG(ADCx->CFGR,
                 ADC_CFGR_JDISCEN
                 | ADC_CFGR_JAUTO
                 ,
                 ADC_INJ_InitStruct->SequencerDiscont
                 | ADC_INJ_InitStruct->TrigAuto
                );
    }
    else
    {
      MODIFY_REG(ADCx->CFGR,
                 ADC_CFGR_JDISCEN
                 | ADC_CFGR_JAUTO
                 ,
                 LL_ADC_REG_SEQ_DISCONT_DISABLE
                 | ADC_INJ_InitStruct->TrigAuto
                );
    }

    MODIFY_REG(ADCx->JSQR,
               ADC_JSQR_JEXTSEL
               | ADC_JSQR_JEXTEN
               | ADC_JSQR_JL
               ,
               ADC_INJ_InitStruct->TriggerSource
               | ADC_INJ_InitStruct->SequencerLength
              );
  }
  else
  {
    /* Initialization error: ADC instance is not disabled. */
    status = ERROR;
  }
  return status;
}

/**
  * @brief  Set each @ref LL_ADC_INJ_InitTypeDef field to default value.
  * @param  ADC_INJ_InitStruct Pointer to a @ref LL_ADC_INJ_InitTypeDef structure
  *                            whose fields will be set to default values.
  * @retval None
  */
void LL_ADC_INJ_StructInit(LL_ADC_INJ_InitTypeDef *ADC_INJ_InitStruct)
{
  /* Set ADC_INJ_InitStruct fields to default values */
  /* Set fields of ADC group injected */
  ADC_INJ_InitStruct->TriggerSource    = LL_ADC_INJ_TRIG_SOFTWARE;
  ADC_INJ_InitStruct->SequencerLength  = LL_ADC_INJ_SEQ_SCAN_DISABLE;
  ADC_INJ_InitStruct->SequencerDiscont = LL_ADC_INJ_SEQ_DISCONT_DISABLE;
  ADC_INJ_InitStruct->TrigAuto         = LL_ADC_INJ_TRIG_INDEPENDENT;
}

#endif
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* ADC1 */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */
