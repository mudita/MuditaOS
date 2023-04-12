/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_sdmmc_host.h"
#include "fsl_sdmmc_common.h"
#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#if !(defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
#include "fsl_cache.h"
#endif
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT (~0U)
#define SDMMCHOST_TRANSFER_CMD_EVENT                                                                                   \
    SDMMC_OSA_EVENT_TRANSFER_CMD_SUCCESS | SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL | SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS | \
        SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL
#define SDMMCHOST_TRANSFER_DATA_EVENT SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS | SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief SDMMCHOST detect card insert status by host controller.
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_DetectCardInsertByHost(USDHC_Type *base, void *userData);

/*!
 * @brief SDMMCHOST detect card remove status by host controller.
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_DetectCardRemoveByHost(USDHC_Type *base, void *userData);

/*!
 * @brief SDMMCHOST transfer complete callback.
 * @param base host base address.
 * @param handle host handle.
 * @param status interrupt status.
 * @param userData user data.
 */
static void SDMMCHOST_TransferCompleteCallback(USDHC_Type *base,
                                               usdhc_handle_t *handle,
                                               status_t status,
                                               void *userData);

/*!
 * @brief SDMMCHOST error recovery.
 * @param base host base address.
 */
static void SDMMCHOST_ErrorRecovery(USDHC_Type *base);

#if SDMMCHOST_SUPPORT_SDR104 || SDMMCHOST_SUPPORT_SDR50 || SDMMCHOST_SUPPORT_HS200 || SDMMCHOST_SUPPORT_HS400
/*!
 * @brief SDMMCHOST execute manual tuning.
 * @param host host handler.
 * @param tuningCmd tuning command
 * @param revBuf receive buffer pointer
 * @param blockSize receive block size
 */
static status_t SDMMCHOST_ExecuteManualTuning(sdmmchost_t *host,
                                              uint32_t tuningCmd,
                                              uint32_t *revBuf,
                                              uint32_t blockSize);
#endif
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static void SDMMCHOST_DetectCardInsertByHost(USDHC_Type *base, void *userData)
{
    sd_detect_card_t *cd = NULL;

    (void)SDMMC_OSAEventSet(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED);
    (void)SDMMC_OSAEventClear(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_REMOVED);

    if (userData != NULL)
    {
        cd = (sd_detect_card_t *)(((sdmmchost_t *)userData)->cd);
        if (cd != NULL)
        {
            if (cd->callback != NULL)
            {
                cd->callback(true, cd->userData);
            }
            if (cd->type == kSD_DetectCardByHostDATA3)
            {
                USDHC_DisableInterruptSignal(base, kUSDHC_CardInsertionFlag);
                if (cd->dat3PullFunc != NULL)
                {
                    cd->dat3PullFunc(kSD_DAT3PullUp);
                }
            }
            else
            {
                USDHC_EnableInterruptSignal(base, kUSDHC_CardRemovalFlag);
            }
        }
    }
}

static void SDMMCHOST_DetectCardRemoveByHost(USDHC_Type *base, void *userData)
{
    sd_detect_card_t *cd = NULL;

    (void)SDMMC_OSAEventSet(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_REMOVED);
    (void)SDMMC_OSAEventClear(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED);

    if (userData != NULL)
    {
        cd = (sd_detect_card_t *)(((sdmmchost_t *)userData)->cd);
        if (cd != NULL)
        {
            if (cd->callback != NULL)
            {
                cd->callback(false, cd->userData);
            }

            if (cd->type == kSD_DetectCardByHostDATA3)
            {
                USDHC_DisableInterruptSignal(base, kUSDHC_CardRemovalFlag);
                if (cd->dat3PullFunc != NULL)
                {
                    cd->dat3PullFunc(kSD_DAT3PullUp);
                }
            }
            else
            {
                USDHC_EnableInterruptSignal(base, kUSDHC_CardInsertionFlag);
            }
        }
    }
}

static void SDMMCHOST_CardInterrupt(USDHC_Type *base, void *userData)
{
    sdio_card_int_t *cardInt = NULL;

    /* application callback */
    if (userData != NULL)
    {
        cardInt = ((sdmmchost_t *)userData)->cardInt;
        if ((cardInt != NULL) && (cardInt->cardInterrupt != NULL))
        {
            cardInt->cardInterrupt(cardInt->userData);
        }
    }
}

status_t SDMMCHOST_CardIntInit(sdmmchost_t *host, void *sdioInt)
{
    host->cardInt                       = sdioInt;
    host->handle.callback.SdioInterrupt = SDMMCHOST_CardInterrupt;
    SDMMCHOST_EnableCardInt(host, true);

    return kStatus_Success;
}

status_t SDMMCHOST_CardDetectInit(sdmmchost_t *host, void *cd)
{
    USDHC_Type *base       = host->hostController.base;
    sd_detect_card_t *sdCD = (sd_detect_card_t *)cd;
    if ((cd == NULL) || ((sdCD->type != kSD_DetectCardByHostDATA3) && (sdCD->type != kSD_DetectCardByHostCD)))
    {
        return kStatus_Fail;
    }

    host->cd                           = cd;
    host->handle.callback.CardInserted = SDMMCHOST_DetectCardInsertByHost;
    host->handle.callback.CardRemoved  = SDMMCHOST_DetectCardRemoveByHost;

    /* enable card detect interrupt */
    USDHC_ClearInterruptStatusFlags(base, (uint32_t)kUSDHC_CardInsertionFlag | (uint32_t)kUSDHC_CardRemovalFlag);
    USDHC_EnableInterruptStatus(base, (uint32_t)kUSDHC_CardInsertionFlag | (uint32_t)kUSDHC_CardRemovalFlag);

    if (SDMMCHOST_CardDetectStatus(host) == (uint32_t)kSD_Inserted)
    {
        (void)SDMMC_OSAEventSet(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED);
        /* notify application about the card insertion status */
        if (sdCD->callback != NULL)
        {
            sdCD->callback(true, sdCD->userData);
        }
    }
    else
    {
        (void)SDMMC_OSAEventSet(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_REMOVED);
        USDHC_EnableInterruptSignal(base, kUSDHC_CardInsertionFlag);
    }

    return kStatus_Success;
}

uint32_t SDMMCHOST_CardDetectStatus(sdmmchost_t *host)
{
    sd_detect_card_t *sdCD = (sd_detect_card_t *)(host->cd);
    uint32_t insertStatus  = kSD_Removed;

    if (sdCD->type == kSD_DetectCardByHostDATA3)
    {
        if (sdCD->dat3PullFunc != NULL)
        {
            sdCD->dat3PullFunc(kSD_DAT3PullDown);
            SDMMC_OSADelay(SDMMCHOST_DATA3_DETECT_CARD_DELAY);
        }
        USDHC_CardDetectByData3(host->hostController.base, true);
        /* Added 1ms delay after host enabled the DAT3 function to avoid CPU missing synchronization with host */
        SDMMC_OSADelay(SDMMCHOST_DATA3_DETECT_CARD_DELAY);
    }
    else
    {
        USDHC_CardDetectByData3(host->hostController.base, false);
    }

    if ((USDHC_GetPresentStatusFlags(host->hostController.base) & (uint32_t)kUSDHC_CardInsertedFlag) != 0U)
    {
        insertStatus = kSD_Inserted;

        if (sdCD->type == kSD_DetectCardByHostDATA3)
        {
            if (sdCD->dat3PullFunc != NULL)
            {
                sdCD->dat3PullFunc(kSD_DAT3PullUp);
            }
            /* disable the DAT3 card detec function */
            USDHC_CardDetectByData3(host->hostController.base, false);
        }
    }

    return insertStatus;
}

status_t SDMMCHOST_PollingCardDetectStatus(sdmmchost_t *host, uint32_t waitCardStatus, uint32_t timeout)
{
    assert(host != NULL);
    assert(host->cd != NULL);

    sd_detect_card_t *cd = host->cd;
    uint32_t event       = 0U;

    if (((waitCardStatus == (uint32_t)kSD_Inserted) && (SDMMCHOST_CardDetectStatus(host) == (uint32_t)kSD_Inserted)) ||
        (((waitCardStatus == (uint32_t)kSD_Removed) && SDMMCHOST_CardDetectStatus(host) == (uint32_t)kSD_Removed)))
    {
        return kStatus_Success;
    }

    (void)SDMMC_OSAEventClear(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED | SDMMC_OSA_EVENT_CARD_REMOVED);

    if (cd->type == kSD_DetectCardByHostDATA3)
    {
        if (cd->dat3PullFunc != NULL)
        {
            cd->dat3PullFunc(kSD_DAT3PullDown);
        }
        USDHC_ClearInterruptStatusFlags(host->hostController.base,
                                        (uint32_t)kUSDHC_CardInsertionFlag | (uint32_t)kUSDHC_CardRemovalFlag);
        USDHC_EnableInterruptSignal(host->hostController.base, waitCardStatus == (uint32_t)kSD_Inserted ?
                                                                   (uint32_t)kUSDHC_CardInsertionFlag :
                                                                   (uint32_t)kUSDHC_CardRemovalFlag);
    }

    /* Wait card inserted. */
    do
    {
        if (SDMMC_OSAEventWait(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED | SDMMC_OSA_EVENT_CARD_REMOVED,
                               timeout, &event) != kStatus_Success)
        {
            return kStatus_Fail;
        }
        else
        {
            if ((waitCardStatus == (uint32_t)kSD_Inserted) &&
                ((event & SDMMC_OSA_EVENT_CARD_INSERTED) == SDMMC_OSA_EVENT_CARD_INSERTED))
            {
                SDMMC_OSADelay(cd->cdDebounce_ms);
                if (SDMMCHOST_CardDetectStatus(host) == (uint32_t)kSD_Inserted)
                {
                    break;
                }
            }

            if (((event & SDMMC_OSA_EVENT_CARD_REMOVED) == SDMMC_OSA_EVENT_CARD_REMOVED) &&
                (waitCardStatus == (uint32_t)kSD_Removed))
            {
                break;
            }
        }
    } while (true);

    return kStatus_Success;
}

void SDMMCHOST_ConvertDataToLittleEndian(sdmmchost_t *host, uint32_t *data, uint32_t wordSize, uint32_t format)
{
    uint32_t temp = 0U;

    if (((uint32_t)host->hostController.config.endianMode == (uint32_t)kSDMMCHOST_EndianModeLittle) &&
        (format == (uint32_t)kSDMMC_DataPacketFormatMSBFirst))
    {
        for (uint32_t i = 0U; i < wordSize; i++)
        {
            temp    = data[i];
            data[i] = SWAP_WORD_BYTE_SEQUENCE(temp);
        }
    }
    else if ((uint32_t)host->hostController.config.endianMode == (uint32_t)kSDMMCHOST_EndianModeHalfWordBig)
    {
        for (uint32_t i = 0U; i < wordSize; i++)
        {
            temp    = data[i];
            data[i] = SWAP_HALF_WROD_BYTE_SEQUENCE(temp);
        }
    }
    else if (((uint32_t)host->hostController.config.endianMode == (uint32_t)kSDMMCHOST_EndianModeBig) &&
             (format == (uint32_t)kSDMMC_DataPacketFormatLSBFirst))
    {
        for (uint32_t i = 0U; i < wordSize; i++)
        {
            temp    = data[i];
            data[i] = SWAP_WORD_BYTE_SEQUENCE(temp);
        }
    }
    else
    {
        /* nothing to do */
    }
}

static void SDMMCHOST_TransferCompleteCallback(USDHC_Type *base,
                                               usdhc_handle_t *handle,
                                               status_t status,
                                               void *userData)
{
    uint32_t eventStatus = 0U;

    if (status == kStatus_USDHC_TransferDataFailed)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL;
    }
    else if (status == kStatus_USDHC_TransferDataComplete)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS;
    }
    else if (status == kStatus_USDHC_SendCommandFailed)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL;
    }
    else if (status == kStatus_USDHC_TransferDMAComplete)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_DMA_COMPLETE;
    }
    else
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_CMD_SUCCESS;
    }

    (void)SDMMC_OSAEventSet(&(((sdmmchost_t *)userData)->hostEvent), eventStatus);
}

#if defined SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER && SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
void SDMMCHOST_InstallCacheAlignBuffer(sdmmchost_t *host, void *cacheAlignBuffer, uint32_t cacheAlignBufferSize)
{
    assert(((uint32_t)cacheAlignBuffer & (SDMMC_DATA_BUFFER_ALIGN_CACHE - 1)) == 0U);
    assert(cacheAlignBufferSize >= SDMMC_DATA_BUFFER_ALIGN_CACHE * 2U);

    host->cacheAlignBuffer     = cacheAlignBuffer;
    host->cacheAlignBufferSize = cacheAlignBufferSize;
}
#endif

status_t SDMMCHOST_TransferFunction(sdmmchost_t *host, sdmmchost_transfer_t *content)
{
    status_t error = kStatus_Success;
    uint32_t event = 0U;
    usdhc_adma_config_t dmaConfig;

#if defined SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER && SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
    usdhc_scatter_gather_data_list_t sgDataList0;
    usdhc_scatter_gather_data_list_t sgDataList1;
    usdhc_scatter_gather_data_t scatterGatherData;
    uint32_t unAlignSize                     = 0U;
    usdhc_scatter_gather_transfer_t transfer = {.data = NULL, .command = content->command};
#endif

    (void)SDMMC_OSAMutexLock(&host->lock, osaWaitForever_c);

    if (content->data != NULL)
    {
        (void)memset(&dmaConfig, 0, sizeof(usdhc_adma_config_t));
        /* config adma */
        dmaConfig.dmaMode = SDMMCHOST_DMA_MODE;
#if !(defined(FSL_FEATURE_USDHC_HAS_NO_RW_BURST_LEN) && FSL_FEATURE_USDHC_HAS_NO_RW_BURST_LEN)
        dmaConfig.burstLen = kUSDHC_EnBurstLenForINCR;
#endif
        dmaConfig.admaTable      = host->dmaDesBuffer;
        dmaConfig.admaTableWords = host->dmaDesBufferWordsNum;

#if defined SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER && SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER

        if ((host->cacheAlignBuffer == NULL) || ((host->cacheAlignBufferSize == 0U)))
        {
            /* application should register cache line size align buffer for host driver maintain the unalign data
             * transfer */
            assert(false);
            return kStatus_InvalidArgument;
        }

        scatterGatherData.enableAutoCommand12 = content->data->enableAutoCommand12;
        scatterGatherData.enableAutoCommand23 = content->data->enableAutoCommand23;
        scatterGatherData.enableIgnoreError   = content->data->enableIgnoreError;
        scatterGatherData.dataType            = content->data->dataType;
        scatterGatherData.blockSize           = content->data->blockSize;

        transfer.data = &scatterGatherData;

        if (content->data->rxData != NULL)
        {
            scatterGatherData.sgData.dataAddr = content->data->rxData;
            scatterGatherData.dataDirection   = kUSDHC_TransferDirectionReceive;
        }
        else
        {
            scatterGatherData.sgData.dataAddr = (uint32_t *)content->data->txData;
            scatterGatherData.dataDirection   = kUSDHC_TransferDirectionSend;
        }
        scatterGatherData.sgData.dataSize = content->data->blockSize * content->data->blockCount;
        scatterGatherData.sgData.dataList = NULL;

        /*
         * If the receive transfer buffer address is not cache line size align, such as
         *---------------------------------------------------------------------
         *|  unalign head region|      align region     |   unaling tail region|
         *---------------------------------------------------------------------
         *
         * Then host driver will splict it into three scatter gather transfers,
         * 1. host->cacheAlignBuffer will be used as first scatter gather data address, the data size is the unalign
         *head region size.
         * 2. align region start address will be used as second scatter gather data address, the data size is the align
         *region size.
         * 3. (uint32_t *)((uint32_t)host->cacheAlignBuffer + SDMMC_DATA_BUFFER_ALIGN_CACHE)
         *    will be used as third scatter gather data address, the data size is the unaling tail region size.
         *
         * Once scatter gather transfer done,
         * 1. host driver will invalidate the cache for the data buffer in the scatter gather transfer list
         * 2. host driver will copy data from host->cacheAlignBuffer to unalign head region
         * 3. host driver will copy data from (uint32_t *)((uint32_t)host->cacheAlignBuffer +
         *SDMMC_DATA_BUFFER_ALIGN_CACHE) to unalign tail region
         *
         * At last, cache line unalign transfer done
         */
        if ((content->data->rxData != NULL) &&
            (((uint32_t)content->data->rxData % SDMMC_DATA_BUFFER_ALIGN_CACHE) != 0U))
        {
            unAlignSize          = ((uint32_t)content->data->rxData -
                           (((uint32_t)content->data->rxData) & (~(SDMMC_DATA_BUFFER_ALIGN_CACHE - 1))));
            sgDataList1.dataSize = unAlignSize;
            unAlignSize          = SDMMC_DATA_BUFFER_ALIGN_CACHE - unAlignSize;

            scatterGatherData.sgData.dataAddr = host->cacheAlignBuffer;
            scatterGatherData.sgData.dataSize = unAlignSize;
            scatterGatherData.sgData.dataList = &sgDataList0;

            sgDataList0.dataAddr =
                (void *)((((uint32_t)content->data->rxData) & (~(SDMMC_DATA_BUFFER_ALIGN_CACHE - 1))) +
                         SDMMC_DATA_BUFFER_ALIGN_CACHE);
            sgDataList0.dataSize = content->data->blockCount * content->data->blockSize - SDMMC_DATA_BUFFER_ALIGN_CACHE;
            sgDataList0.dataList = &sgDataList1;
            sgDataList1.dataAddr = (uint32_t *)((uint32_t)host->cacheAlignBuffer + SDMMC_DATA_BUFFER_ALIGN_CACHE);
            sgDataList1.dataList = NULL;
        }
#endif

#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#if !(defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
        if (host->enableCacheControl == kSDMMCHOST_CacheControlRWBuffer)
        {
            /* no matter read or write transfer, clean the cache line anyway to avoid data miss */
            DCACHE_CleanByRange(
                (uint32_t)(content->data->txData == NULL ? content->data->rxData : content->data->txData),
                (content->data->blockSize) * (content->data->blockCount));
        }
#endif
#endif
    }

    /* clear redundant transfer event flag */
    (void)SDMMC_OSAEventClear(&(host->hostEvent), SDMMCHOST_TRANSFER_CMD_EVENT);

#if defined SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER && SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
    error = USDHC_TransferScatterGatherADMANonBlocking(host->hostController.base, &host->handle, &dmaConfig, &transfer);
#else
    error = USDHC_TransferNonBlocking(host->hostController.base, &host->handle, &dmaConfig, content);
#endif

    if (error == kStatus_Success)
    {
        /* wait command event */
        if ((kStatus_Fail == SDMMC_OSAEventWait(&(host->hostEvent), SDMMCHOST_TRANSFER_CMD_EVENT,
                                                SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT, &event)) ||
            ((event & SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL) != 0U))
        {
            error = kStatus_Fail;
        }
        else
        {
            if (content->data != NULL)
            {
                if ((event & SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS) == 0U)
                {
                    if (((event & SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL) != 0U) ||
                        (kStatus_Fail == SDMMC_OSAEventWait(&(host->hostEvent), SDMMCHOST_TRANSFER_DATA_EVENT,
                                                            SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT, &event) ||
                         ((event & SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL) != 0U)))
                    {
                        error = kStatus_Fail;
                    }
                }
            }
        }
    }

    if (error != kStatus_Success)
    {
        /* host error recovery */
        SDMMCHOST_ErrorRecovery(host->hostController.base);
    }
    else
    {
        if ((content->data != NULL) && (content->data->rxData != NULL))
        {
#if defined SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER && SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
            if (((uint32_t)content->data->rxData % SDMMC_DATA_BUFFER_ALIGN_CACHE) != 0U)
            {
#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#if !(defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
                if (host->enableCacheControl == kSDMMCHOST_CacheControlRWBuffer)
                {
                    DCACHE_InvalidateByRange((uint32_t)scatterGatherData.sgData.dataAddr,
                                             scatterGatherData.sgData.dataSize);

                    DCACHE_InvalidateByRange((uint32_t)sgDataList0.dataAddr, sgDataList0.dataSize);

                    DCACHE_InvalidateByRange((uint32_t)sgDataList1.dataAddr, sgDataList1.dataSize);
                }
#endif
#endif
                memcpy(content->data->rxData, scatterGatherData.sgData.dataAddr, scatterGatherData.sgData.dataSize);
                memcpy((void *)((uint32_t)content->data->rxData + content->data->blockCount * content->data->blockSize -
                                sgDataList1.dataSize),
                       sgDataList1.dataAddr, sgDataList1.dataSize);
            }
            else
#endif
            {
#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#if !(defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
                /* invalidate the cache for read */
                if (host->enableCacheControl == kSDMMCHOST_CacheControlRWBuffer)
                {
                    DCACHE_InvalidateByRange((uint32_t)content->data->rxData,
                                             (content->data->blockSize) * (content->data->blockCount));
                }
#endif
#endif
            }
        }
    }

    (void)SDMMC_OSAMutexUnlock(&host->lock);

    return error;
}

static void SDMMCHOST_ErrorRecovery(USDHC_Type *base)
{
    uint32_t status = 0U;
    /* get host present status */
    status = USDHC_GetPresentStatusFlags(base);
    /* check command inhibit status flag */
    if ((status & (uint32_t)kUSDHC_CommandInhibitFlag) != 0U)
    {
        /* reset command line */
        (void)USDHC_Reset(base, kUSDHC_ResetCommand, 100U);
    }
    /* check data inhibit status flag */
    if (((status & (uint32_t)kUSDHC_DataInhibitFlag) != 0U) || (USDHC_GetAdmaErrorStatusFlags(base) != 0U))
    {
        /* reset data line */
        (void)USDHC_Reset(base, kUSDHC_ResetData, 100U);
    }
}

void SDMMCHOST_SetCardPower(sdmmchost_t *host, bool enable)
{
    /* host not support */
}

void SDMMCHOST_SetCardBusWidth(sdmmchost_t *host, uint32_t dataBusWidth)
{
    USDHC_SetDataBusWidth(host->hostController.base, dataBusWidth == (uint32_t)kSDMMC_BusWdith1Bit ?
                                                         kUSDHC_DataBusWidth1Bit :
                                                         dataBusWidth == (uint32_t)kSDMMC_BusWdith4Bit ?
                                                         kUSDHC_DataBusWidth4Bit :
                                                         kUSDHC_DataBusWidth8Bit);
}

status_t SDMMCHOST_Init(sdmmchost_t *host)
{
    assert(host != NULL);

    usdhc_transfer_callback_t usdhcCallback = {0};
    usdhc_host_t *usdhcHost                 = &(host->hostController);
#if defined FSL_FEATURE_USDHC_INSTANCE_SUPPORT_8_BIT_WIDTHn
    uint32_t bus8bitCapability = (uint32_t)FSL_FEATURE_USDHC_INSTANCE_SUPPORT_8_BIT_WIDTHn(host->hostController.base);
#else
    uint32_t bus8bitCapability    = 0U;
#endif

#if (defined(FSL_FEATURE_USDHC_HAS_HS400_MODE) && (FSL_FEATURE_USDHC_HAS_HS400_MODE))
#if defined FSL_FEATURE_USDHC_INSTANCE_SUPPORT_HS400_MODEn
    uint32_t hs400Capability = (uint32_t)FSL_FEATURE_USDHC_INSTANCE_SUPPORT_HS400_MODEn(host->hostController.base);
#else
    uint32_t hs400Capability = 0U;
#endif
#endif

#if defined FSL_FEATURE_USDHC_INSTANCE_SUPPORT_1V8_SIGNALn
    uint32_t voltage1v8Capability = (uint32_t)FSL_FEATURE_USDHC_INSTANCE_SUPPORT_1V8_SIGNALn(host->hostController.base);
#else
    uint32_t voltage1v8Capability = 0U;
#endif
    status_t error = kStatus_Success;
    /* sdmmc osa init */

    host->capability = (uint32_t)kSDMMCHOST_SupportHighSpeed | (uint32_t)kSDMMCHOST_SupportSuspendResume |
                       (uint32_t)kSDMMCHOST_SupportVoltage3v3 | (uint32_t)kSDMMCHOST_SupportVoltage1v8 |
                       (uint32_t)kSDMMCHOST_SupportVoltage1v2 | (uint32_t)kSDMMCHOST_Support4BitDataWidth |
                       (uint32_t)kSDMMCHOST_SupportDDRMode | (uint32_t)kSDMMCHOST_SupportDetectCardByData3 |
                       (uint32_t)kSDMMCHOST_SupportDetectCardByCD | (uint32_t)kSDMMCHOST_SupportAutoCmd12;

    if (bus8bitCapability != 0U)
    {
        host->capability |= (uint32_t)kSDMMCHOST_Support8BitDataWidth;
    }

    if (voltage1v8Capability != 0U)
    {
#if (defined(FSL_FEATURE_USDHC_HAS_SDR104_MODE) && (FSL_FEATURE_USDHC_HAS_SDR104_MODE))
        host->capability |= (uint32_t)kSDMMCHOST_SupportSDR104;
#endif

#if (defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (FSL_FEATURE_USDHC_HAS_SDR50_MODE))
        host->capability |= (uint32_t)kSDMMCHOST_SupportSDR50 | (uint32_t)kSDMMCHOST_SupportHS200;
#endif
    }

#if (defined(FSL_FEATURE_USDHC_HAS_HS400_MODE) && (FSL_FEATURE_USDHC_HAS_HS400_MODE))
    if (hs400Capability != 0U)
    {
        host->capability |= (uint32_t)kSDMMCHOST_SupportHS400;
    }
#endif
    host->maxBlockCount = SDMMCHOST_SUPPORT_MAX_BLOCK_COUNT;
    host->maxBlockSize  = SDMMCHOST_SUPPORT_MAX_BLOCK_LENGTH;

    (void)SDMMC_OSAMutexCreate(&host->lock);
    (void)SDMMC_OSAMutexLock(&host->lock, osaWaitForever_c);

    /* Initializes USDHC. */
    usdhcHost->config.endianMode          = kUSDHC_EndianModeLittle;
    usdhcHost->config.dataTimeout         = 0xFU;
    usdhcHost->config.readWatermarkLevel  = 0x80U;
    usdhcHost->config.writeWatermarkLevel = 0x80U;
    USDHC_Init(usdhcHost->base, &(usdhcHost->config));

    /* Create handle for SDHC driver */
    usdhcCallback.TransferComplete = SDMMCHOST_TransferCompleteCallback;
    USDHC_TransferCreateHandle(usdhcHost->base, &host->handle, &usdhcCallback, host);

    /* Create transfer event. */
    if (kStatus_Success != SDMMC_OSAEventCreate(&(host->hostEvent)))
    {
        error = kStatus_Fail;
    }

    (void)SDMMC_OSAMutexUnlock(&host->lock);

    return error;
}

void SDMMCHOST_Reset(sdmmchost_t *host)
{
    USDHC_Type *base = host->hostController.base;

    (void)SDMMC_OSAMutexLock(&host->lock, osaWaitForever_c);

#if SDMMCHOST_SUPPORT_VOLTAGE_CONTROL
    /* voltage switch to normal but not 1.8V */
    UDSHC_SelectVoltage(base, false);
#endif
    /* Disable DDR mode */
    USDHC_EnableDDRMode(base, false, 0U);
    /* disable tuning */
#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (FSL_FEATURE_USDHC_HAS_SDR50_MODE)
    USDHC_EnableStandardTuning(base, 0, 0, false);
    USDHC_EnableAutoTuning(host->hostController.base, false);
#endif

#if FSL_FEATURE_USDHC_HAS_HS400_MODE
    /* Disable HS400 mode */
    USDHC_EnableHS400Mode(base, false);
    /* Disable DLL */
    USDHC_EnableStrobeDLL(base, false);
#endif
    /* reset data/command/tuning circuit */
    (void)USDHC_Reset(base, kUSDHC_ResetAll, 100U);

    USDHC_DisableInterruptSignal(base, kUSDHC_AllInterruptFlags);

    (void)SDMMC_OSAMutexUnlock(&host->lock);
}

void SDMMCHOST_Deinit(sdmmchost_t *host)
{
    (void)SDMMC_OSAMutexLock(&host->lock, osaWaitForever_c);
    usdhc_host_t *sdhcHost = &host->hostController;
    SDMMCHOST_Reset(host);
    USDHC_Deinit(sdhcHost->base);
    (void)SDMMC_OSAEventDestroy(&(host->hostEvent));
    (void)SDMMC_OSAMutexDestroy(&host->lock);
}

void SDMMCHOST_SwitchToVoltage(sdmmchost_t *host, uint32_t voltage)
{
#if !(defined(FSL_FEATURE_USDHC_HAS_NO_VOLTAGE_SELECT) && (FSL_FEATURE_USDHC_HAS_NO_VOLTAGE_SELECT))
    if (voltage == (uint32_t)kSDMMC_OperationVoltage180V)
    {
        UDSHC_SelectVoltage(host->hostController.base, true);
    }
    else
    {
        UDSHC_SelectVoltage(host->hostController.base, false);
    }
#endif
}

#if SDMMCHOST_SUPPORT_SDR104 || SDMMCHOST_SUPPORT_SDR50 || SDMMCHOST_SUPPORT_HS200 || SDMMCHOST_SUPPORT_HS400
static status_t SDMMCHOST_ExecuteStdTuning(sdmmchost_t *host, uint32_t tuningCmd, uint32_t *revBuf, uint32_t blockSize)
{
    sdmmchost_transfer_t content = {0U};
    sdmmchost_cmd_t command      = {0U};
    sdmmchost_data_t data        = {0U};
    bool tuningError             = true;
    status_t error               = kStatus_Success;

    command.index        = tuningCmd;
    command.argument     = 0U;
    command.responseType = kCARD_ResponseTypeR1;

    data.blockSize  = blockSize;
    data.blockCount = 1U;
    data.rxData     = revBuf;
    data.dataType   = kUSDHC_TransferDataTuning;

    content.command = &command;
    content.data    = &data;

    (void)USDHC_Reset(host->hostController.base, kUSDHC_ResetTuning, 100U);
    USDHC_ForceClockOn(host->hostController.base, true);
    /* disable standard tuning */
    USDHC_EnableStandardTuning(host->hostController.base, SDMMCHOST_STANDARD_TUNING_START, SDMMCHOST_TUINIG_STEP,
                               false);
    /*
     * Tuning fail found on some SOCS caused by the difference of delay cell, so we need to i
     * ncrease the tuning counter to cover the adjustable tuninig window
     */
    USDHC_SetStandardTuningCounter(host->hostController.base, SDMMCHOST_STANDARD_TUNING_COUNTER);
    /* enable the standard tuning */
    USDHC_EnableStandardTuning(host->hostController.base, SDMMCHOST_STANDARD_TUNING_START, SDMMCHOST_TUINIG_STEP, true);

    while (true)
    {
        error = SDMMCHOST_TransferFunction(host, &content);
        /* send tuning block */
        if (kStatus_Success != error)
        {
            return kStatus_SDMMC_TransferFailed;
        }
        SDMMC_OSADelay(1U);

        /*wait excute tuning bit clear*/
        if ((USDHC_GetExecuteStdTuningStatus(host->hostController.base) != 0U))
        {
            continue;
        }

        /* if tuning error , re-tuning again */
        if ((USDHC_CheckTuningError(host->hostController.base) != 0U) && tuningError)
        {
            tuningError = false;
            /* enable the standard tuning */
            USDHC_EnableStandardTuning(host->hostController.base, SDMMCHOST_STANDARD_TUNING_START,
                                       SDMMCHOST_TUINIG_STEP, true);
            (void)USDHC_SetTuningDelay(host->hostController.base, SDMMCHOST_STANDARD_TUNING_START, 0U, 0U);
        }
        else
        {
            break;
        }
    }

    /* check tuning result*/
    if (USDHC_CheckStdTuningResult(host->hostController.base) == 0U)
    {
        return kStatus_SDMMC_TuningFail;
    }
    USDHC_ForceClockOn(host->hostController.base, false);
    USDHC_EnableAutoTuning(host->hostController.base, true);

    return kStatus_Success;
}

static status_t SDMMC_CheckTuningResult(uint32_t *tuningWindow, uint32_t *validWindowStart, uint32_t *validWindowEnd)
{
    uint32_t tempValidWindowLen = 0U, tempValidWindowStart = 0U, tempValidWindowEnd = 0U;
    uint32_t validWindowLenMax = 0U, ValidWindowStartMax = 0U, validWindowEndMax = 0U;

    for (uint32_t i = 0U; i < SDMMCHOST_MAX_TUNING_DELAY_CELL; i++)
    {
        if ((tuningWindow[i / 32U] & (1UL << (i % 32U))) != 0U)
        {
            if (tempValidWindowLen == 0U)
            {
                tempValidWindowStart = i;
            }
            tempValidWindowLen++;
        }
        else
        {
            if (tempValidWindowLen != 0U)
            {
                tempValidWindowEnd = i - 1U;

#if defined SDMMC_ENABLE_LOG_PRINT
                SDMMC_LOG("valid tuning window start: %d, end: %d\r\n", tempValidWindowStart, tempValidWindowEnd);
#endif
                if (tempValidWindowLen > validWindowLenMax)
                {
                    validWindowLenMax   = tempValidWindowLen;
                    ValidWindowStartMax = tempValidWindowStart;
                    validWindowEndMax   = tempValidWindowEnd;
                }
                tempValidWindowLen = 0U;
            }
        }
    }

    if (validWindowLenMax == 0U)
    {
        return kStatus_Fail;
    }

    *validWindowStart = ValidWindowStartMax;
    *validWindowEnd   = validWindowEndMax;

    return kStatus_Success;
}

static status_t SDMMCHOST_ExecuteManualTuning(sdmmchost_t *host,
                                              uint32_t tuningCmd,
                                              uint32_t *revBuf,
                                              uint32_t blockSize)
{
    uint32_t *buffer         = revBuf;
    status_t ret             = kStatus_Success;
    uint32_t tuningDelayCell = 0U;
    uint32_t tuningWindow[4] = {0U}, tuningWindowStart = 0U, tuningWindowEnd = 0U;

    sdmmchost_transfer_t content = {0U};
    sdmmchost_cmd_t command      = {0U};
    sdmmchost_data_t data        = {0U};

    command.index        = tuningCmd;
    command.argument     = 0U;
    command.responseType = kCARD_ResponseTypeR1;

    data.blockSize  = blockSize;
    data.blockCount = 1U;
    data.rxData     = revBuf;

    content.command = &command;
    content.data    = &data;

    (void)USDHC_Reset(host->hostController.base, kUSDHC_ResetAll, 100U);
    USDHC_EnableManualTuning(host->hostController.base, true);
    USDHC_ForceClockOn(host->hostController.base, true);

    while (true)
    {
        (void)USDHC_SetTuningDelay(host->hostController.base, tuningDelayCell, 0U, 0U);

        if ((SDMMCHOST_TransferFunction(host, &content) == kStatus_Success) &&
            (((uint32_t)kUSDHC_TuningPassFlag & USDHC_GetInterruptStatusFlags(host->hostController.base)) != 0U))
        {
            USDHC_ClearInterruptStatusFlags(host->hostController.base, kUSDHC_TuningPassFlag);
            tuningWindow[tuningDelayCell / 32U] |= 1UL << (tuningDelayCell % 32U);

#if defined SDMMC_ENABLE_LOG_PRINT
            SDMMC_LOG("tuning pass point: %d\r\n", tuningDelayCell);
#endif
        }
        else
        {
#if defined SDMMC_ENABLE_LOG_PRINT
            SDMMC_LOG("tuning fail point: %d\r\n", tuningDelayCell);
#endif
        }

        if (++tuningDelayCell >= SDMMCHOST_MAX_TUNING_DELAY_CELL)
        {
            break;
        }

        (void)memset(buffer, 0, blockSize);

        SDMMC_OSADelay(2U);
    }

    /* After the whole 0-128 delay cell validated, tuning result information stored in tuningWindow, this function will
    check the valid winddow and will select a longest window as the final tuning delay setting */
    if (SDMMC_CheckTuningResult(tuningWindow, &tuningWindowStart, &tuningWindowEnd) == kStatus_Fail)
    {
        return kStatus_Fail;
    }

    /* abort tuning */
    USDHC_EnableManualTuning(host->hostController.base, false);
    USDHC_ForceClockOn(host->hostController.base, false);
    (void)USDHC_Reset(host->hostController.base, kUSDHC_ResetAll, 100U);

    /* select middle position of the window */
    (void)USDHC_SetTuningDelay(host->hostController.base, (tuningWindowStart + tuningWindowEnd) / 2U - 3U, 3U, 3U);
    tuningDelayCell = ((tuningWindowStart + tuningWindowEnd) / 2U - 3U) << 8U | 0x33U;
    /* wait the tuning delay value write successfully */
    while ((USDHC_GetTuningDelayStatus(host->hostController.base) & tuningDelayCell) != tuningDelayCell)
    {
    }
    /* enable auto tuning */
    USDHC_EnableAutoTuning(host->hostController.base, true);

    return ret;
}
#endif

status_t SDMMCHOST_ExecuteTuning(sdmmchost_t *host, uint32_t tuningCmd, uint32_t *revBuf, uint32_t blockSize)
{
#if SDMMCHOST_SUPPORT_SDR104 || SDMMCHOST_SUPPORT_SDR50 || SDMMCHOST_SUPPORT_HS200 || SDMMCHOST_SUPPORT_HS400
    if (host->tuningType == (uint32_t)kSDMMCHOST_StandardTuning)
    {
        return SDMMCHOST_ExecuteStdTuning(host, tuningCmd, revBuf, blockSize);
    }

    return SDMMCHOST_ExecuteManualTuning(host, tuningCmd, revBuf, blockSize);
#else
    return kStatus_SDMMC_NotSupportYet;
#endif
}

status_t SDMMCHOST_StartBoot(sdmmchost_t *host,
                             sdmmchost_boot_config_t *hostConfig,
                             sdmmchost_cmd_t *cmd,
                             uint8_t *buffer)
{
    sdmmchost_transfer_t content = {0};
    sdmmchost_data_t data        = {0};
    status_t error               = kStatus_Success;

    USDHC_SetMmcBootConfig(host->hostController.base, hostConfig);

    data.blockSize  = hostConfig->blockSize;
    data.blockCount = hostConfig->blockCount;
    data.rxData     = (uint32_t *)(uint32_t)buffer;
    data.dataType   = kUSDHC_TransferDataBoot;

    content.data    = &data;
    content.command = cmd;

    error = SDMMCHOST_TransferFunction(host, &content);
    /* should check tuning error during every transfer*/
    if (kStatus_Success != error)
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t SDMMCHOST_ReadBootData(sdmmchost_t *host, sdmmchost_boot_config_t *hostConfig, uint8_t *buffer)
{
    sdmmchost_cmd_t command      = {0};
    sdmmchost_transfer_t content = {0};
    sdmmchost_data_t data        = {0};
    status_t error               = kStatus_Success;

    USDHC_SetMmcBootConfig(host->hostController.base, hostConfig);
    USDHC_EnableMmcBoot(host->hostController.base, true);

    data.blockSize  = hostConfig->blockSize;
    data.blockCount = hostConfig->blockCount;
    data.rxData     = (uint32_t *)(uint32_t)buffer;
    data.dataType   = kUSDHC_TransferDataBootcontinous;
    /* no command should be send out  */
    command.type = kCARD_CommandTypeEmpty;

    content.data    = &data;
    content.command = &command;

    error = SDMMCHOST_TransferFunction(host, &content);
    if (kStatus_Success != error)
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}
