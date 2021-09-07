/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_sdmmc_host.h"
#include "board.h"
#include "fsl_sdmmc_event.h"
#include "fsl_gpio.h"
#ifdef BOARD_USDHC_CD_PORT_BASE
#include "fsl_port.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief SDMMCHOST transfer function.
 * @param base host base address.
 * @param content transfer configurations.
 */
static status_t SDMMCHOST_TransferFunction(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER *content);

/*!
 * @brief SDMMCHOST transfer complete callback.
 * @param base host base address.
 * @param handle host handle.
 * @param status interrupt status.
 * @param userData user data.
 */
static void SDMMCHOST_TransferCompleteCallback(SDMMCHOST_TYPE *base,
                                               usdhc_handle_t *handle,
                                               status_t status,
                                               void *userData);

/*!
 * @brief SDMMCHOST re-tuning callback
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_ReTuningCallback(SDMMCHOST_TYPE *base, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* DMA descriptor should allocate at non-cached memory */
// TODO: check this !! I don't think this structure needs alignment and special cache treatment
AT_NONCACHEABLE_SECTION_ALIGN(uint32_t g_usdhcAdma2Table[USDHC_ADMA_TABLE_WORDS], USDHC_ADMA2_ADDR_ALIGN);

usdhc_handle_t g_usdhcHandle;
volatile bool g_usdhcTransferSuccessFlag = true;
static volatile bool s_sdInsertedFlag    = false;
volatile status_t g_reTuningFlag         = false;

/* This variables exists only for testing purposes. I will leave it until code matures. */
static uint8_t transfer_in_progress = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/
static void SDMMCHOST_TransferCompleteCallback(SDMMCHOST_TYPE *base,
                                               usdhc_handle_t *handle,
                                               status_t status,
                                               void *userData)
{
    /* wait the target status and then notify the transfer complete */
    if (status == kStatus_Success) {
        g_usdhcTransferSuccessFlag = true;
    }
    else {
        g_usdhcTransferSuccessFlag = false;
    }

    transfer_in_progress = 0;
    portEND_SWITCHING_ISR(SDMMCEVENT_Notify(status));
}

// TODO: this need to be checked when using HS200
static void SDMMCHOST_ReTuningCallback(SDMMCHOST_TYPE *base, void *userData)
{
    g_reTuningFlag = true;
    // SDMMCEVENT_Notify(kSDMMCEVENT_TransferComplete);
}

extern uint32_t cntnotify;
extern uint32_t cntwait;

static status_t SDMMCHOST_TransferFunction(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER *content)
{
    status_t error = kStatus_Success;

    usdhc_adma_config_t dmaConfig;

    if (SDMMCEVENT_Lock(500) == false) {
        return kStatus_Timeout;
    }

    if (transfer_in_progress) {
        assert(0);
    }
    transfer_in_progress = 1;

    assert(cntnotify == cntwait);

    if (content->data != NULL) {
        memset(&dmaConfig, 0, sizeof(usdhc_adma_config_t));
        /* config adma */
        dmaConfig.dmaMode        = USDHC_DMA_MODE;
        dmaConfig.burstLen       = kUSDHC_EnBurstLenForINCR;
        dmaConfig.admaTable      = g_usdhcAdma2Table;
        dmaConfig.admaTableWords = USDHC_ADMA_TABLE_WORDS;
    }

    do {
        error = USDHC_TransferNonBlocking(base, &g_usdhcHandle, &dmaConfig, content);
    } while (error == kStatus_USDHC_BusyTransferring);

    if (error != kStatus_Success) {
        SDMMCEVENT_Delete();
        /* host error recovery */
        SDMMCHOST_ErrorRecovery(base);
        SDMMCEVENT_Unlock();
        return error;
    }

    bool ret = SDMMCEVENT_Wait(SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT);

    if ((!ret) || (g_usdhcTransferSuccessFlag == false)) {
        SDMMCEVENT_Delete();
        /* host error recovery */
        SDMMCHOST_ErrorRecovery(base);
        SDMMCEVENT_Unlock();
        return kStatus_Fail;
    }

    SDMMCEVENT_Unlock();

    return error;
}

void SDMMCHOST_ErrorRecovery(SDMMCHOST_TYPE *base)
{
    uint32_t status = 0U;
    /* get host present status */
    status = USDHC_GetPresentStatusFlags(base);
    /* check command inhibit status flag */
    if ((status & kUSDHC_CommandInhibitFlag) != 0U) {
        /* reset command line */
        USDHC_Reset(base, kUSDHC_ResetCommand, 100U);
    }
    /* check data inhibit status flag */
    if ((status & kUSDHC_DataInhibitFlag) != 0U) {
        /* reset data line */
        USDHC_Reset(base, kUSDHC_ResetData, 100U);
    }
}

void SDMMCHOST_Delay(uint32_t milliseconds)
{
    SDMMCEVENT_Delay(milliseconds);
}

void SDMMCHOST_PowerOffCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    if (pwr != NULL) {
        pwr->powerOff();
        SDMMCHOST_Delay(pwr->powerOffDelay_ms);
    }
    else {
        /* only SD card need card detect*/
        SDMMCHOST_ENABLE_SD_POWER(false);
        /* Delay several milliseconds to make card stable. */
        SDMMCHOST_Delay(100U);
    }
}

void SDMMCHOST_PowerOnCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    /* use user define the power on function  */
    if (pwr != NULL) {
        pwr->powerOn();
        SDMMCHOST_Delay(pwr->powerOnDelay_ms);
    }
    else {
        /* card power on */
        SDMMCHOST_ENABLE_SD_POWER(true);
        /* Delay several milliseconds to make card stable. */
        SDMMCHOST_Delay(300U);
    }
}

status_t SDMMCHOST_Init(SDMMCHOST_CONFIG *host, void *userData)
{
    usdhc_host_t *usdhcHost            = (usdhc_host_t *)host;
    usdhc_transfer_callback_t callback = {
        .TransferComplete = SDMMCHOST_TransferCompleteCallback,
        .ReTuning         = SDMMCHOST_ReTuningCallback,
        .CardInserted     = NULL, /* Not used for eMMC */
        .CardRemoved      = NULL, /* Not used for eMMC */
        .SdioInterrupt    = NULL,
        .BlockGap         = NULL,
    };
    /* init card power control */
    SDMMCHOST_INIT_SD_POWER();

    SDMMCHOST_PowerOffCard(host->base, NULL);
    SDMMCHOST_PowerOnCard(host->base, NULL);

    /* Initializes USDHC. */
    usdhcHost->config.dataTimeout         = USDHC_DATA_TIMEOUT;
    usdhcHost->config.endianMode          = USDHC_ENDIAN_MODE;
    usdhcHost->config.readWatermarkLevel  = USDHC_READ_WATERMARK_LEVEL;
    usdhcHost->config.writeWatermarkLevel = USDHC_WRITE_WATERMARK_LEVEL;
    usdhcHost->config.readBurstLen        = USDHC_READ_BURST_LEN;
    usdhcHost->config.writeBurstLen       = USDHC_WRITE_BURST_LEN;

    USDHC_Init(usdhcHost->base, &(usdhcHost->config));

    /* disable the card insert/remove interrupt, due to use GPIO interrupt detect card */
    USDHC_DisableInterruptSignal(usdhcHost->base, kUSDHC_CardRemovalFlag | kUSDHC_CardInsertionFlag);
    // TODO: change USDHC1_IRQn to sth different
    SDMMCHOST_SET_IRQ_PRIORITY(USDHC2_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
    /* create interrupt handler */
    USDHC_TransferCreateHandle(usdhcHost->base, &g_usdhcHandle, &callback, userData);
    /* Create transfer complete event. */
    if (false == SDMMCEVENT_Create()) {
        return kStatus_Fail;
    }

    /* Define transfer function. */
    usdhcHost->transfer = SDMMCHOST_TransferFunction;

    return kStatus_Success;
}

void SDMMCHOST_Reset(SDMMCHOST_TYPE *base)
{
    /* voltage switch to normal but not 1.8V */
    SDMMCHOST_SWITCH_VOLTAGE180V(base, false);
    /* Disable DDR mode */
    SDMMCHOST_ENABLE_DDR_MODE(base, false, 0U);
    /* disable tuning */
    SDMMCHOST_EXECUTE_STANDARD_TUNING_ENABLE(base, false);
    /* Disable HS400 mode */
    SDMMCHOST_ENABLE_HS400_MODE(base, false);
    /* Disable DLL */
    SDMMCHOST_ENABLE_STROBE_DLL(base, false);
}

void SDMMCHOST_Deinit(void *host)
{
    usdhc_host_t *usdhcHost = (usdhc_host_t *)host;
    SDMMCHOST_Reset(usdhcHost->base);
    USDHC_Deinit(usdhcHost->base);
}
