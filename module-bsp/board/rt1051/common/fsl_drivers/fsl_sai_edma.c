/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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

#include "fsl_sai_edma.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.sai_edma"
#endif

/*******************************************************************************
 * Definitations
 ******************************************************************************/
/* Used for 32byte aligned */
#define STCD_ADDR(address) (edma_tcd_t *)(((uint32_t)(address) + 32) & ~0x1FU)

static I2S_Type *const s_saiBases[] = I2S_BASE_PTRS;

/*<! Structure definition for uart_edma_private_handle_t. The structure is private. */
typedef struct _sai_edma_private_handle
{
    I2S_Type *base;
    sai_edma_handle_t *handle;
} sai_edma_private_handle_t;

enum _sai_edma_transfer_state
{
    kSAI_Busy = 0x0U, /*!< SAI is busy */
    kSAI_Idle,        /*!< Transfer is done. */
};

/*<! Private handle only used for internally. */
static sai_edma_private_handle_t s_edmaPrivateHandle[ARRAY_SIZE(s_saiBases)][2];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the instance number for SAI.
 *
 * @param base SAI base pointer.
 */
static uint32_t SAI_GetInstance(I2S_Type *base);

/*!
 * @brief SAI EDMA callback for send.
 *
 * @param handle pointer to sai_edma_handle_t structure which stores the transfer state.
 * @param userData Parameter for user callback.
 * @param done If the DMA transfer finished.
 * @param tcds The TCD index.
 */
static void SAI_TxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds);

/*!
 * @brief SAI EDMA callback for receive.
 *
 * @param handle pointer to sai_edma_handle_t structure which stores the transfer state.
 * @param userData Parameter for user callback.
 * @param done If the DMA transfer finished.
 * @param tcds The TCD index.
 */
static void SAI_RxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds);

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t SAI_GetInstance(I2S_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_saiBases); instance++) {
        if (s_saiBases[instance] == base) {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_saiBases));

    return instance;
}

static void SAI_TxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds)
{
    sai_edma_private_handle_t *privHandle = (sai_edma_private_handle_t *)userData;
    sai_edma_handle_t *saiHandle          = privHandle->handle;

    /* If finished a blcok, call the callback function */
    memset(&saiHandle->saiQueue[saiHandle->queueDriver], 0, sizeof(sai_transfer_t));
    saiHandle->queueDriver = (saiHandle->queueDriver + 1) % SAI_XFER_QUEUE_SIZE;
    if (saiHandle->callback) {
        (saiHandle->callback)(privHandle->base, saiHandle, kStatus_SAI_TxIdle, saiHandle->userData);
    }

    /* If all data finished, just stop the transfer */
    if (saiHandle->saiQueue[saiHandle->queueDriver].data == NULL) {
        /* Disable DMA enable bit */
        SAI_TxEnableDMA(privHandle->base, kSAI_FIFORequestDMAEnable, false);
        EDMA_AbortTransfer(handle);
    }
}

static void SAI_RxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds)
{
    sai_edma_private_handle_t *privHandle = (sai_edma_private_handle_t *)userData;
    sai_edma_handle_t *saiHandle          = privHandle->handle;

    /* If finished a blcok, call the callback function */
    memset(&saiHandle->saiQueue[saiHandle->queueDriver], 0, sizeof(sai_transfer_t));
    saiHandle->queueDriver = (saiHandle->queueDriver + 1) % SAI_XFER_QUEUE_SIZE;
    if (saiHandle->callback) {
        (saiHandle->callback)(privHandle->base, saiHandle, kStatus_SAI_RxIdle, saiHandle->userData);
    }

    /* If all data finished, just stop the transfer */
    if (saiHandle->saiQueue[saiHandle->queueDriver].data == NULL) {
        /* Disable DMA enable bit */
        SAI_RxEnableDMA(privHandle->base, kSAI_FIFORequestDMAEnable, false);
        EDMA_AbortTransfer(handle);
    }
}

void SAI_TransferTxCreateHandleEDMA(
    I2S_Type *base, sai_edma_handle_t *handle, sai_edma_callback_t callback, void *userData, edma_handle_t *dmaHandle)
{
    assert(handle && dmaHandle);

    uint32_t instance = SAI_GetInstance(base);

    /* Zero the handle */
    memset(handle, 0, sizeof(*handle));

    /* Set sai base to handle */
    handle->dmaHandle = dmaHandle;
    handle->callback  = callback;
    handle->userData  = userData;

    /* Set SAI state to idle */
    handle->state = kSAI_Idle;

    s_edmaPrivateHandle[instance][0].base   = base;
    s_edmaPrivateHandle[instance][0].handle = handle;

    /* Need to use scatter gather */
    EDMA_InstallTCDMemory(dmaHandle, (edma_tcd_t *)(STCD_ADDR(handle->tcd)), SAI_XFER_QUEUE_SIZE);

    /* Install callback for Tx dma channel */
    EDMA_SetCallback(dmaHandle, SAI_TxEDMACallback, &s_edmaPrivateHandle[instance][0]);
}

void SAI_TransferRxCreateHandleEDMA(
    I2S_Type *base, sai_edma_handle_t *handle, sai_edma_callback_t callback, void *userData, edma_handle_t *dmaHandle)
{
    assert(handle && dmaHandle);

    uint32_t instance = SAI_GetInstance(base);

    /* Zero the handle */
    memset(handle, 0, sizeof(*handle));

    /* Set sai base to handle */
    handle->dmaHandle = dmaHandle;
    handle->callback  = callback;
    handle->userData  = userData;

    /* Set SAI state to idle */
    handle->state = kSAI_Idle;

    s_edmaPrivateHandle[instance][1].base   = base;
    s_edmaPrivateHandle[instance][1].handle = handle;

    /* Need to use scatter gather */
    EDMA_InstallTCDMemory(dmaHandle, STCD_ADDR(handle->tcd), SAI_XFER_QUEUE_SIZE);

    /* Install callback for Tx dma channel */
    EDMA_SetCallback(dmaHandle, SAI_RxEDMACallback, &s_edmaPrivateHandle[instance][1]);
}

void SAI_TransferTxSetFormatEDMA(I2S_Type *base,
                                 sai_edma_handle_t *handle,
                                 sai_transfer_format_t *format,
                                 uint32_t mclkSourceClockHz,
                                 uint32_t bclkSourceClockHz)
{
    assert(handle && format);

    /* Configure the audio format to SAI registers */
    SAI_TxSetFormat(base, format, mclkSourceClockHz, bclkSourceClockHz);

    /* Get the tranfer size from format, this should be used in EDMA configuration */
    if (format->bitWidth == 24U) {
        handle->bytesPerFrame = 4U;
    }
    else {
        handle->bytesPerFrame = format->bitWidth / 8U;
    }

    /* Update the data channel SAI used */
    handle->channel = format->channel;

    /* Clear the channel enable bits unitl do a send/receive */
    base->TCR3 &= ~I2S_TCR3_TCE_MASK;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    handle->count = FSL_FEATURE_SAI_FIFO_COUNT - format->watermark;
#else
    handle->count = 1U;
#endif /* FSL_FEATURE_SAI_FIFO_COUNT */
}

void SAI_TransferRxSetFormatEDMA(I2S_Type *base,
                                 sai_edma_handle_t *handle,
                                 sai_transfer_format_t *format,
                                 uint32_t mclkSourceClockHz,
                                 uint32_t bclkSourceClockHz)
{
    assert(handle && format);

    /* Configure the audio format to SAI registers */
    SAI_RxSetFormat(base, format, mclkSourceClockHz, bclkSourceClockHz);

    /* Get the tranfer size from format, this should be used in EDMA configuration */
    if (format->bitWidth == 24U) {
        handle->bytesPerFrame = 4U;
    }
    else {
        handle->bytesPerFrame = format->bitWidth / 8U;
    }

    /* Update the data channel SAI used */
    handle->channel = format->channel;

    /* Clear the channel enable bits unitl do a send/receive */
    base->RCR3 &= ~I2S_RCR3_RCE_MASK;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    handle->count = format->watermark;
#else
    handle->count = 1U;
#endif /* FSL_FEATURE_SAI_FIFO_COUNT */
}

status_t SAI_TransferSendEDMA(I2S_Type *base, sai_edma_handle_t *handle, sai_transfer_t *xfer)
{
    assert(handle && xfer);

    edma_transfer_config_t config = {0};
    uint32_t destAddr             = SAI_TxGetDataRegisterAddress(base, handle->channel);

    /* Check if input parameter invalid */
    if ((xfer->data == NULL) || (xfer->dataSize == 0U)) {
        return kStatus_InvalidArgument;
    }

    if (handle->saiQueue[handle->queueUser].data) {
        return kStatus_SAI_QueueFull;
    }

    /* Change the state of handle */
    handle->state = kSAI_Busy;

    /* Update the queue state */
    handle->transferSize[handle->queueUser]      = xfer->dataSize;
    handle->saiQueue[handle->queueUser].data     = xfer->data;
    handle->saiQueue[handle->queueUser].dataSize = xfer->dataSize;
    handle->queueUser                            = (handle->queueUser + 1) % SAI_XFER_QUEUE_SIZE;

    /* Prepare edma configure */
    EDMA_PrepareTransfer(&config,
                         xfer->data,
                         handle->bytesPerFrame,
                         (void *)destAddr,
                         handle->bytesPerFrame,
                         handle->count * handle->bytesPerFrame,
                         xfer->dataSize,
                         kEDMA_MemoryToPeripheral);

    /* Store the initially configured eDMA minor byte transfer count into the SAI handle */
    handle->nbytes = handle->count * handle->bytesPerFrame;

    EDMA_SubmitTransfer(handle->dmaHandle, &config);

    /* Start DMA transfer */
    EDMA_StartTransfer(handle->dmaHandle);

    /* Enable DMA enable bit */
    SAI_TxEnableDMA(base, kSAI_FIFORequestDMAEnable, true);

    /* Enable SAI Tx clock */
    SAI_TxEnable(base, true);

    /* Enable the channel FIFO */
    base->TCR3 |= I2S_TCR3_TCE(1U << handle->channel);

    return kStatus_Success;
}

status_t SAI_TransferReceiveEDMA(I2S_Type *base, sai_edma_handle_t *handle, sai_transfer_t *xfer)
{
    assert(handle && xfer);

    edma_transfer_config_t config = {0};
    uint32_t srcAddr              = SAI_RxGetDataRegisterAddress(base, handle->channel);

    /* Check if input parameter invalid */
    if ((xfer->data == NULL) || (xfer->dataSize == 0U)) {
        return kStatus_InvalidArgument;
    }

    if (handle->saiQueue[handle->queueUser].data) {
        return kStatus_SAI_QueueFull;
    }

    /* Change the state of handle */
    handle->state = kSAI_Busy;

    /* Update queue state  */
    handle->transferSize[handle->queueUser]      = xfer->dataSize;
    handle->saiQueue[handle->queueUser].data     = xfer->data;
    handle->saiQueue[handle->queueUser].dataSize = xfer->dataSize;
    handle->queueUser                            = (handle->queueUser + 1) % SAI_XFER_QUEUE_SIZE;

    /* Prepare edma configure */
    EDMA_PrepareTransfer(&config,
                         (void *)srcAddr,
                         handle->bytesPerFrame,
                         xfer->data,
                         handle->bytesPerFrame,
                         handle->count * handle->bytesPerFrame,
                         xfer->dataSize,
                         kEDMA_PeripheralToMemory);

    /* Store the initially configured eDMA minor byte transfer count into the SAI handle */
    handle->nbytes = handle->count * handle->bytesPerFrame;

    EDMA_SubmitTransfer(handle->dmaHandle, &config);

    /* Start DMA transfer */
    EDMA_StartTransfer(handle->dmaHandle);

    /* Enable DMA enable bit */
    SAI_RxEnableDMA(base, kSAI_FIFORequestDMAEnable, true);

    /* Enable the channel FIFO */
    base->RCR3 |= I2S_RCR3_RCE(1U << handle->channel);

    /* Enable SAI Rx clock */
    SAI_RxEnable(base, true);

    return kStatus_Success;
}

void SAI_TransferAbortSendEDMA(I2S_Type *base, sai_edma_handle_t *handle)
{
    assert(handle);

    /* Disable dma */
    EDMA_AbortTransfer(handle->dmaHandle);

    /* Disable the channel FIFO */
    base->TCR3 &= ~I2S_TCR3_TCE_MASK;

    /* Disable DMA enable bit */
    SAI_TxEnableDMA(base, kSAI_FIFORequestDMAEnable, false);

    /* Disable Tx */
    SAI_TxEnable(base, false);

    /* Reset the FIFO pointer, at the same time clear all error flags if set */
    base->TCSR |= (I2S_TCSR_FR_MASK | I2S_TCSR_SR_MASK);
    base->TCSR &= ~I2S_TCSR_SR_MASK;

    /* Handle the queue index */
    memset(&handle->saiQueue[handle->queueDriver], 0, sizeof(sai_transfer_t));
    handle->queueDriver = (handle->queueDriver + 1) % SAI_XFER_QUEUE_SIZE;

    /* Set the handle state */
    handle->state = kSAI_Idle;
}

void SAI_TransferAbortReceiveEDMA(I2S_Type *base, sai_edma_handle_t *handle)
{
    assert(handle);

    /* Disable dma */
    EDMA_AbortTransfer(handle->dmaHandle);

    /* Disable the channel FIFO */
    base->RCR3 &= ~I2S_RCR3_RCE_MASK;

    /* Disable DMA enable bit */
    SAI_RxEnableDMA(base, kSAI_FIFORequestDMAEnable, false);

    /* Disable Rx */
    SAI_RxEnable(base, false);

    /* Reset the FIFO pointer, at the same time clear all error flags if set */
    base->RCSR |= (I2S_RCSR_FR_MASK | I2S_RCSR_SR_MASK);
    base->RCSR &= ~I2S_RCSR_SR_MASK;

    /* Handle the queue index */
    memset(&handle->saiQueue[handle->queueDriver], 0, sizeof(sai_transfer_t));
    handle->queueDriver = (handle->queueDriver + 1) % SAI_XFER_QUEUE_SIZE;

    /* Set the handle state */
    handle->state = kSAI_Idle;
}

void SAI_TransferTerminateSendEDMA(I2S_Type *base, sai_edma_handle_t *handle)
{
    assert(handle);

    /* Abort the current transfer */
    SAI_TransferAbortSendEDMA(base, handle);

    /* Clear all the internal information */
    memset(handle->tcd, 0U, sizeof(handle->tcd));
    memset(handle->saiQueue, 0U, sizeof(handle->saiQueue));
    memset(handle->transferSize, 0U, sizeof(handle->transferSize));
    handle->queueUser   = 0U;
    handle->queueDriver = 0U;
}

void SAI_TransferTerminateReceiveEDMA(I2S_Type *base, sai_edma_handle_t *handle)
{
    assert(handle);

    /* Abort the current transfer */
    SAI_TransferAbortReceiveEDMA(base, handle);

    /* Clear all the internal information */
    memset(handle->tcd, 0U, sizeof(handle->tcd));
    memset(handle->saiQueue, 0U, sizeof(handle->saiQueue));
    memset(handle->transferSize, 0U, sizeof(handle->transferSize));
    handle->queueUser   = 0U;
    handle->queueDriver = 0U;
}

status_t SAI_TransferGetSendCountEDMA(I2S_Type *base, sai_edma_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kSAI_Busy) {
        status = kStatus_NoTransferInProgress;
    }
    else {
        *count = (handle->transferSize[handle->queueDriver] -
                  (uint32_t)handle->nbytes *
                      EDMA_GetRemainingMajorLoopCount(handle->dmaHandle->base, handle->dmaHandle->channel));
    }

    return status;
}

status_t SAI_TransferGetReceiveCountEDMA(I2S_Type *base, sai_edma_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kSAI_Busy) {
        status = kStatus_NoTransferInProgress;
    }
    else {
        *count = (handle->transferSize[handle->queueDriver] -
                  (uint32_t)handle->nbytes *
                      EDMA_GetRemainingMajorLoopCount(handle->dmaHandle->base, handle->dmaHandle->channel));
    }

    return status;
}
