/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SDMMC_HOST_H
#define _FSL_SDMMC_HOST_H

#include "fsl_common.h"
#include "fsl_sdmmc_osa.h"
#include "fsl_usdhc.h"

/*!
 * @addtogroup sdmmchost_usdhc
 * @ingroup sdmmchost
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Middleware adapter version. */
#define FSL_SDMMC_HOST_ADAPTER_VERSION (MAKE_VERSION(2U, 6U, 3U)) /*2.6.3*/

#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#define SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER 0
#if SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
#if !(defined FSL_USDHC_ENABLE_SCATTER_GATHER_TRANSFER && FSL_USDHC_ENABLE_SCATTER_GATHER_TRANSFER)
#error "missing definition of macro FSL_USDHC_ENABLE_SCATTER_GATHER_TRANSFER"
#endif
#endif
#endif

/*! @brief sdmmc host capability */
enum
{
    kSDMMCHOST_SupportHighSpeed         = 1U << 0U,  /*!< high speed capability */
    kSDMMCHOST_SupportSuspendResume     = 1U << 1U,  /*!< suspend resume capability */
    kSDMMCHOST_SupportVoltage3v3        = 1U << 2U,  /*!< 3V3 capability */
    kSDMMCHOST_SupportVoltage3v0        = 1U << 3U,  /*!< 3V0 capability */
    kSDMMCHOST_SupportVoltage1v8        = 1U << 4U,  /*!< 1V8 capability */
    kSDMMCHOST_SupportVoltage1v2        = 1U << 5U,  /*!< 1V2 capability */
    kSDMMCHOST_Support4BitDataWidth     = 1U << 6U,  /*!< 4 bit data width capability */
    kSDMMCHOST_Support8BitDataWidth     = 1U << 7U,  /*!< 8 bit data width capability */
    kSDMMCHOST_SupportDDRMode           = 1U << 8U,  /*!< DDR mode capability */
    kSDMMCHOST_SupportDetectCardByData3 = 1U << 9U,  /*!< data3 detect card capability */
    kSDMMCHOST_SupportDetectCardByCD    = 1U << 10U, /*!< CD detect card capability */
    kSDMMCHOST_SupportAutoCmd12         = 1U << 11U, /*!< auto command 12 capability */
    kSDMMCHOST_SupportSDR104            = 1U << 12U, /*!< SDR104 capability */
    kSDMMCHOST_SupportSDR50             = 1U << 13U, /*!< SDR50 capability */
    kSDMMCHOST_SupportHS200             = 1U << 14U, /*!< HS200 capability */
    kSDMMCHOST_SupportHS400             = 1U << 15U, /*!< HS400 capability */
};

/*!@brief sdmmc host misc capability */
#define SDMMCHOST_SUPPORT_HIGH_SPEED           (1U)
#define SDMMCHOST_SUPPORT_SUSPEND_RESUME       (1U)
#define SDMMCHOST_SUPPORT_VOLTAGE_3V3          (1U)
#define SDMMCHOST_SUPPORT_VOLTAGE_3V0          (0U)
#define SDMMCHOST_SUPPORT_VOLTAGE_1V8          (1U)
#define SDMMCHOST_SUPPORT_VOLTAGE_1V2          (1U)
#define SDMMCHOST_SUPPORT_4_BIT_WIDTH          (1U)
#define SDMMCHOST_SUPPORT_8_BIT_WIDTH          (1U)
#define SDMMCHOST_SUPPORT_DDR_MODE             (1U)
#define SDMMCHOST_SUPPORT_DETECT_CARD_BY_DATA3 (1U)
#define SDMMCHOST_SUPPORT_DETECT_CARD_BY_CD    (1U)
#define SDMMCHOST_SUPPORT_AUTO_CMD12           (1U)
#define SDMMCHOST_SUPPORT_MAX_BLOCK_LENGTH     (4096U)
#define SDMMCHOST_SUPPORT_MAX_BLOCK_COUNT      (USDHC_MAX_BLOCK_COUNT)
#if !(defined(FSL_FEATURE_USDHC_HAS_NO_VOLTAGE_SELECT) && (FSL_FEATURE_USDHC_HAS_NO_VOLTAGE_SELECT))
#define SDMMCHOST_SUPPORT_VOLTAGE_CONTROL (1)
#else
#define SDMMCHOST_SUPPORT_VOLTAGE_CONTROL (0)
#endif
/*! @brief sdmmc host sdcard DDR50 mode capability*/
#define SDMMCHOST_SUPPORT_DDR50 (SDMMCHOST_SUPPORT_DDR_MODE)
/*! @brief sdmmc host sdcard SDR50 mode capability*/
#if (defined(FSL_FEATURE_USDHC_HAS_SDR104_MODE) && (FSL_FEATURE_USDHC_HAS_SDR104_MODE))
#define SDMMCHOST_SUPPORT_SDR104 (1U)
#else
#define SDMMCHOST_SUPPORT_SDR104 (0U)
#endif
/*! @brief sdmmc host sdcard SDR104/mmccard HS200 mode capability*/
#if (defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (FSL_FEATURE_USDHC_HAS_SDR50_MODE))
#define SDMMCHOST_SUPPORT_SDR50 (1U)
#define SDMMCHOST_SUPPORT_HS200 (1U)
#else
#define SDMMCHOST_SUPPORT_HS200 (0U)
#define SDMMCHOST_SUPPORT_SDR50 (0U)
#endif
/*! @brief sdmmc host mmccard HS400 mode capability*/
#if (defined(FSL_FEATURE_USDHC_HAS_HS400_MODE) && (FSL_FEATURE_USDHC_HAS_HS400_MODE))
#define SDMMCHOST_SUPPORT_HS400 (1U)
#else
#define SDMMCHOST_SUPPORT_HS400 (0U)
#endif
/*! @brief sdmmc host instance capability */
#if defined FSL_FEATURE_USDHC_INSTANCE_SUPPORT_8_BIT_WIDTHn
#define SDMMCHOST_INSTANCE_SUPPORT_8_BIT_WIDTH(host) \
    FSL_FEATURE_USDHC_INSTANCE_SUPPORT_8_BIT_WIDTHn(host->hostController.base)
#else
#define SDMMCHOST_INSTANCE_SUPPORT_8_BIT_WIDTH(host) 0
#endif
#if defined FSL_FEATURE_USDHC_INSTANCE_SUPPORT_HS400_MODEn
#define SDMMCHOST_INSTANCE_SUPPORT_HS400(host) FSL_FEATURE_USDHC_INSTANCE_SUPPORT_HS400_MODEn(host->hostController.base)
#else
#define SDMMCHOST_INSTANCE_SUPPORT_HS400(host) 0
#endif
#if defined FSL_FEATURE_USDHC_INSTANCE_SUPPORT_1V8_SIGNALn
#define SDMMCHOST_INSTANCE_SUPPORT_1V8_SIGNAL(host) \
    FSL_FEATURE_USDHC_INSTANCE_SUPPORT_1V8_SIGNALn(host->hostController.base)
#define SDMMCHOST_INSTANCE_SUPPORT_HS200(host)  SDMMCHOST_INSTANCE_SUPPORT_1V8_SIGNAL(host)
#define SDMMCHOST_INSTANCE_SUPPORT_SDR104(host) SDMMCHOST_INSTANCE_SUPPORT_1V8_SIGNAL(host)
#define SDMMCHOST_INSTANCE_SUPPORT_SDR50(host)  SDMMCHOST_INSTANCE_SUPPORT_1V8_SIGNAL(host)
#define SDMMCHOST_INSTANCE_SUPPORT_DDR50(host)  SDMMCHOST_INSTANCE_SUPPORT_1V8_SIGNAL(host)
#else
#define SDMMCHOST_INSTANCE_SUPPORT_1V8_SIGNAL(host) 0
#define SDMMCHOST_INSTANCE_SUPPORT_HS200(host)      0
#define SDMMCHOST_INSTANCE_SUPPORT_SDR104(host)     0
#define SDMMCHOST_INSTANCE_SUPPORT_SDR50(host)      0
#define SDMMCHOST_INSTANCE_SUPPORT_DDR50(host)      0
#endif

#ifndef SDMMCHOST_DMA_MODE
#define SDMMCHOST_DMA_MODE kUSDHC_DmaModeAdma2
#endif
/*! @brief sdmmchost delay for DAT3 detect card */
#ifndef SDMMCHOST_DATA3_DETECT_CARD_DELAY
#define SDMMCHOST_DATA3_DETECT_CARD_DELAY (10U)
#endif
/*!@brief SDMMC host dma descriptor buffer address align size */
#define SDMMCHOST_DMA_DESCRIPTOR_BUFFER_ALIGN_SIZE (4U)
/*!@brief tuning configuration */
#define SDMMCHOST_STANDARD_TUNING_START            (10U) /*!< standard tuning start point */
#define SDMMCHOST_TUINIG_STEP                      (2U)  /*!< standard tuning stBep */
#define SDMMCHOST_STANDARD_TUNING_COUNTER          (60)
#define SDMMCHOST_STROBE_DLL_DELAY_TARGET          (7U)
#define SDMMCHOST_STROBE_DLL_DELAY_UPDATE_INTERVAL (4U)
#define SDMMCHOST_MAX_TUNING_DELAY_CELL            (128U)
/*!@brief sdmmc host transfer function */
typedef usdhc_transfer_t sdmmchost_transfer_t;
typedef usdhc_command_t sdmmchost_cmd_t;
typedef usdhc_data_t sdmmchost_data_t;
typedef struct _sdmmchost_ SDMMCHOST_CONFIG;
typedef USDHC_Type SDMMCHOST_TYPE;
typedef void sdmmchost_detect_card_t;
typedef usdhc_boot_config_t sdmmchost_boot_config_t;
/*! @brief host Endian mode
 * corresponding to driver define
 * @anchor _sdmmchost_endian_mode
 */
enum
{
    kSDMMCHOST_EndianModeBig         = 0U, /*!< Big endian mode */
    kSDMMCHOST_EndianModeHalfWordBig = 1U, /*!< Half word big endian mode */
    kSDMMCHOST_EndianModeLittle      = 2U, /*!< Little endian mode */
};

/*! @brief sdmmc host tuning type
 * @anchor _sdmmchost_tuning_type
 */
enum
{
    kSDMMCHOST_StandardTuning = 0U, /*!< standard tuning type */
    kSDMMCHOST_ManualTuning   = 1U, /*!< manual tuning type */
};

/*! @brief sdmmc host maintain cache flag
 * @anchor _sdmmc_host_cache_control
 */
enum
{
    kSDMMCHOST_NoCacheControl       = 0U, /*!< sdmmc host cache control disabled */
    kSDMMCHOST_CacheControlRWBuffer = 1U, /*!< sdmmc host cache control read/write buffer */
};

/*!@brief sdmmc host handler  */
typedef struct _sdmmchost_
{
    usdhc_host_t hostController;   /*!< host configuration */
    void *dmaDesBuffer;            /*!< DMA descriptor buffer address */
    uint32_t dmaDesBufferWordsNum; /*!< DMA descriptor buffer size in byte */
    usdhc_handle_t handle;         /*!< host controller handler */
    uint32_t capability;           /*!< host controller capability */
    uint32_t maxBlockCount;        /*!< host controller maximum block count */
    uint32_t maxBlockSize;         /*!< host controller maximum block size */

    uint8_t tuningType; /*!< host tuning type */

    sdmmc_osa_event_t hostEvent; /*!< host event handler */
    void *cd;                    /*!< card detect */
    void *cardInt;               /*!< call back function for card interrupt */

#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
    uint8_t enableCacheControl; /*!< Cache maintain flag in host driver. Host driver only maintain cache when
                                   FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL is not defined and enableCacheControl =
                                   kSDMMCHOST_CacheControlRWBuffer. While FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL is
                                   defined, host driver will not maintain cache and peripheral driver will do it.*/
#if defined SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER && SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
    void *cacheAlignBuffer;        /*!< cache line size align buffer */
    uint32_t cacheAlignBufferSize; /*!< cache line size align buffer size, the size must be not smaller than 2 * cache
                                      line size */
#endif
#endif

    sdmmc_osa_mutex_t lock; /*!< host access lock */
} sdmmchost_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name USDHC host controller function
 * @{
 */

/*!
 * @brief set data bus width.
 * @param host host handler
 * @param dataBusWidth data bus width
 */
void SDMMCHOST_SetCardBusWidth(sdmmchost_t *host, uint32_t dataBusWidth);

/*!
 * @brief Send initilization active 80 clocks to card.
 * @param host host handler
 */
static inline void SDMMCHOST_SendCardActive(sdmmchost_t *host)
{
    (void)USDHC_SetCardActive(host->hostController.base, 100U);
}

/*!
 * @brief Set card bus clock.
 * @param host host handler
 * @param targetClock target clock frequency
 * @retval actual clock frequency can be reach.
 */
static inline uint32_t SDMMCHOST_SetCardClock(sdmmchost_t *host, uint32_t targetClock)
{
    return USDHC_SetSdClock(host->hostController.base, host->hostController.sourceClock_Hz, targetClock);
}

/*!
 * @brief check card status by DATA0.
 * @param host host handler
 * @retval true is busy, false is idle.
 */
static inline bool SDMMCHOST_IsCardBusy(sdmmchost_t *host)
{
    return (USDHC_GetPresentStatusFlags(host->hostController.base) & (uint32_t)kUSDHC_Data0LineLevelFlag) ==
                   (uint32_t)kUSDHC_Data0LineLevelFlag ?
               false :
               true;
}

/*!
 * @brief Get signal line status.
 * @param host host handler
 * @param signalLine signal line type, reference _sdmmc_signal_line
 */
static inline uint32_t SDMMCHOST_GetSignalLineStatus(sdmmchost_t *host, uint32_t signalLine)
{
    return (USDHC_GetPresentStatusFlags(host->hostController.base) >> USDHC_PRES_STATE_CLSL_SHIFT) & signalLine;
}
/*!
 * @brief enable card interrupt.
 * @param host host handler
 * @param enable true is enable, false is disable.
 */
static inline void SDMMCHOST_EnableCardInt(sdmmchost_t *host, bool enable)
{
    if (enable)
    {
        USDHC_EnableInterruptStatus(host->hostController.base, kUSDHC_CardInterruptFlag);
        USDHC_EnableInterruptSignal(host->hostController.base, kUSDHC_CardInterruptFlag);
    }
    else
    {
        USDHC_DisableInterruptStatus(host->hostController.base, kUSDHC_CardInterruptFlag);
        USDHC_DisableInterruptSignal(host->hostController.base, kUSDHC_CardInterruptFlag);
    }
}

/*!
 * @brief enable DDR mode.
 * @param host host handler
 * @param enable true is enable, false is disable.
 * @param nibblePos nibble position indictation. 0- the sequence is 'odd high nibble -> even high nibble ->
 * odd low nibble -> even low nibble'; 1- the sequence is 'odd high nibble -> odd low nibble -> even high
 * nibble -> even low nibble'.
 */
static inline void SDMMCHOST_EnableDDRMode(sdmmchost_t *host, bool enable, uint32_t nibblePos)
{
    USDHC_EnableDDRMode(host->hostController.base, enable, nibblePos);
}

/*!
 * @brief enable HS400 mode.
 * @param host host handler
 * @param enable true is enable, false is disable.
 */
static inline void SDMMCHOST_EnableHS400Mode(sdmmchost_t *host, bool enable)
{
#if FSL_FEATURE_USDHC_HAS_HS400_MODE
    assert(SDMMCHOST_INSTANCE_SUPPORT_HS400(host) != 0);

    USDHC_EnableHS400Mode(host->hostController.base, enable);
#endif
}

/*!
 * @brief enable STROBE DLL.
 * @param host host handler
 * @param enable true is enable, false is disable.
 */
static inline void SDMMCHOST_EnableStrobeDll(sdmmchost_t *host, bool enable)
{
#if FSL_FEATURE_USDHC_HAS_HS400_MODE
    if (enable)
    {
        USDHC_ConfigStrobeDLL(host->hostController.base, SDMMCHOST_STROBE_DLL_DELAY_TARGET,
                              SDMMCHOST_STROBE_DLL_DELAY_UPDATE_INTERVAL);
        USDHC_EnableStrobeDLL(host->hostController.base, enable);
    }
    else
    {
        USDHC_EnableStrobeDLL(host->hostController.base, enable);
    }
#endif
}

/*!
 * @brief start read boot data.
 * @param host host handler
 * @param hostConfig boot configuration
 * @param cmd boot command
 * @param buffer buffer address
 */
status_t SDMMCHOST_StartBoot(sdmmchost_t *host,
                             sdmmchost_boot_config_t *hostConfig,
                             sdmmchost_cmd_t *cmd,
                             uint8_t *buffer);

/*!
 * @brief read boot data.
 * @param host host handler
 * @param hostConfig boot configuration
 * @param buffer buffer address
 */
status_t SDMMCHOST_ReadBootData(sdmmchost_t *host, sdmmchost_boot_config_t *hostConfig, uint8_t *buffer);

/*!
 * @brief enable boot mode.
 * @param host host handler
 * @param enable true is enable, false is disable
 */
static inline void SDMMCHOST_EnableBoot(sdmmchost_t *host, bool enable)
{
    USDHC_EnableMmcBoot(host->hostController.base, enable);
}

/*!
 * @brief card interrupt function.
 * @param host host handler
 * @param sdioInt card interrupt configuration
 */
status_t SDMMCHOST_CardIntInit(sdmmchost_t *host, void *sdioInt);

/*!
 * @brief force card clock on.
 * @param host host handler
 * @param enable true is enable, false is disable.
 */
static inline void SDMMCHOST_ForceClockOn(sdmmchost_t *host, bool enable)
{
    USDHC_ForceClockOn(host->hostController.base, enable);
}

/*!
 * @brief switch to voltage.
 * @param host host handler
 * @param voltage switch to voltage level.
 */
void SDMMCHOST_SwitchToVoltage(sdmmchost_t *host, uint32_t voltage);

/*!
 * @brief card detect init function.
 * @param host host handler
 * @param cd card detect configuration
 */
status_t SDMMCHOST_CardDetectInit(sdmmchost_t *host, void *cd);

/*!
 * @brief Detect card insert, only need for SD cases.
 * @param host host handler
 * @param waitCardStatus status which user want to wait
 * @param timeout wait time out.
 * @retval kStatus_Success detect card insert
 * @retval kStatus_Fail card insert event fail
 */
status_t SDMMCHOST_PollingCardDetectStatus(sdmmchost_t *host, uint32_t waitCardStatus, uint32_t timeout);

/*!
 * @brief card detect status.
 * @param host host handler
 * @retval kSD_Inserted, kSD_Removed
 */
uint32_t SDMMCHOST_CardDetectStatus(sdmmchost_t *host);

/*!
 * @brief Init host controller.
 *
 * Thread safe function, please note that the function will create the mutex lock dynamically by default,
 * so to avoid the mutex create redundantly, application must follow bellow sequence for card re-initialization
 * @code
 * SDMMCHOST_Deinit(host);
 * SDMMCHOST_Init(host);
 * @endcode
 *
 * @param host host handler
 * @retval kStatus_Success host init success
 * @retval kStatus_Fail event fail
 */
status_t SDMMCHOST_Init(sdmmchost_t *host);

/*!
 * @brief Deinit host controller.
 * Please note it is a thread safe function.
 *
 * @param host host handler
 */
void SDMMCHOST_Deinit(sdmmchost_t *host);

/*!
 * @brief host power off card function.
 * @param host host handler
 * @param enable true is power on, false is power down.
 */
void SDMMCHOST_SetCardPower(sdmmchost_t *host, bool enable);

#if defined SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER && SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
/*!
 * @brief Install cache line size align buffer for the transfer require cache line size align.
 * @param host host handler
 * @param cacheAlignBuffer cache line size align buffer pointer.
 * @param cacheAlignBufferSize cache line size align buffer size.
 */
void SDMMCHOST_InstallCacheAlignBuffer(sdmmchost_t *host, void *cacheAlignBuffer, uint32_t cacheAlignBufferSize);
#endif

/*!
 * @brief host transfer function.
 *
 * Please note it is a thread safe function.
 *
 * @note the host transfer function support below functionality,
 * 1. Non-cache line size alignment check on the data buffer, it is means that no matter the data buffer used for data
 * transfer is align with cache line size or not, sdmmc host driver will use the address directly.
 * 2. Cache line size alignment check on the data buffer, sdmmc host driver will check the data buffer address, if the
 * buffer is not align with cache line size, sdmmc host driver will convert it to cache line size align buffer, the
 * functionality is enabled by \#define SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER 1 \#define
 * FSL_USDHC_ENABLE_SCATTER_GATHER_TRANSFER 1U If application would like to enable the cache line size align
 * functionality, please make sure the
 *  SDMMCHOST_InstallCacheAlignBuffer is called before submit data transfer request and make sure the installing buffer
 * size is not smaller than 2 * cache line size.
 *
 * @param host host handler
 * @param content transfer content.
 */
status_t SDMMCHOST_TransferFunction(sdmmchost_t *host, sdmmchost_transfer_t *content);

/*!
 * @brief sdmmc host excute tuning.
 *
 * @param host host handler
 * @param tuningCmd tuning command.
 * @param revBuf receive buffer pointer
 * @param blockSize tuning data block size.
 */
status_t SDMMCHOST_ExecuteTuning(sdmmchost_t *host, uint32_t tuningCmd, uint32_t *revBuf, uint32_t blockSize);

/*!
 * @brief host reset function.
 *
 * @param host host handler
 */
void SDMMCHOST_Reset(sdmmchost_t *host);

/*!
 * @brief sdmmc host convert data sequence to little endian sequence
 *
 * @param host host handler.
 * @param data data buffer address.
 * @param wordSize data buffer size in word.
 * @param format data packet format.
 */
void SDMMCHOST_ConvertDataToLittleEndian(sdmmchost_t *host, uint32_t *data, uint32_t wordSize, uint32_t format);
/* @} */

#if defined(__cplusplus)
}
#endif
/* @} */
#endif /* _FSL_SDMMC_HOST_H */
