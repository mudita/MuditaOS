// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverSAI.hpp"
#include "bsp/BoardDefinitions.hpp"

#include "../fsl_drivers/fsl_sai_edma.h"

namespace drivers
{

    using namespace magic_enum;

    sai_edma_handle_t RT1051DriverSAI::txHandle[magic_enum::enum_count<SAIInstances>()] = {};
    sai_edma_handle_t RT1051DriverSAI::rxHandle[magic_enum::enum_count<SAIInstances>()] = {};

    RT1051DriverSAI::RT1051DriverSAI(const drivers::SAIInstances &inst, const drivers::DriverSAIParams &params)
        : DriverSAI(params)
    {
        switch (instances) {
        case SAIInstances ::SAI_1:
            rxDMASource = kDmaRequestMuxSai1Rx;
            txDMASource = kDmaRequestMuxSai1Tx;
            base        = SAI1;
            IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK;
            break;
        case SAIInstances ::SAI_2:
            rxDMASource = kDmaRequestMuxSai2Rx;
            txDMASource = kDmaRequestMuxSai2Tx;
            base        = SAI2;
            IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_SAI2_MCLK_DIR_MASK;
            break;
        case SAIInstances ::SAI_3:
            rxDMASource = kDmaRequestMuxSai3Rx;
            txDMASource = kDmaRequestMuxSai3Tx;
            base        = SAI3;
            IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_SAI3_MCLK_DIR_MASK;
            break;
        }

        pll    = DriverInterface<DriverPLL>::Create(static_cast<PLLInstances>(BoardDefinitions ::AUDIO_PLL),
                                                 DriverPLLParams{});
        dmamux = DriverInterface<DriverDMAMux>::Create(
            static_cast<DMAMuxInstances>(BoardDefinitions ::AUDIOCODEC_DMAMUX), DriverDMAMuxParams{});
        dma = DriverInterface<DriverDMA>::Create(static_cast<DMAInstances>(BoardDefinitions ::AUDIOCODEC_DMA),
                                                 DriverDMAParams{});
    }

    RT1051DriverSAI::~RT1051DriverSAI()
    {}

    int32_t RT1051DriverSAI::StartInTransfer(const drivers::TransferParams &rx)
    {
        sai_transfer_t xfer = {0};
        sai_config_t config;
        sai_transfer_format_t sai_format = {0};

        rxParams = rx;

        dmamux->Enable(parameters.rxDMAChannel, rxDMASource);
        dma->CreateHandle(parameters.rxDMAChannel);

        // Initialize SAI Rx module
        SAI_RxGetDefaultConfig(&config);

        config.masterSlave =
            (parameters.masterslave == DriverSAIParams::MasterSlave ::Slave) ? kSAI_Slave : kSAI_Master;
        SAI_RxInit(base, &config);

        /* Configure the audio format */
        sai_format.bitWidth           = parameters.bitWidth;
        sai_format.channel            = 0U;
        sai_format.sampleRate_Hz      = parameters.sampleRate;
        sai_format.masterClockHz      = parameters.masterClock;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol           = static_cast<sai_protocol_t>(parameters.protocol);
        sai_format.stereo             = static_cast<sai_mono_stereo_t>(parameters.monostereo);
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferRxCreateHandleEDMA(base,
                                       &rxHandle[enum_integer(instances)],
                                       rxAudioCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(dma->GetHandle(parameters.rxDMAChannel)));

        SAI_TransferRxSetFormatEDMA(
            base, &rxHandle[enum_integer(instances)], &sai_format, parameters.masterClock, parameters.masterClock);

        /* Reset SAI Rx internal logic */
        SAI_RxSoftwareReset(base, kSAI_ResetTypeSoftware);

        xfer.data     = rx.data;
        xfer.dataSize = rx.size;
        SAI_TransferReceiveEDMA(base, &rxHandle[enum_integer(instances)], &xfer);
    }

    int32_t RT1051DriverSAI::StopInTransfer()
    {
        SAI_RxDisableInterrupts(base, kSAI_FIFOErrorInterruptEnable);
        if (rxHandle[enum_integer(instances)].dmaHandle) {
            SAI_TransferAbortReceiveEDMA(base, &rxHandle[enum_integer(instances)]);
            memset(&rxHandle[enum_integer(instances)], 0, sizeof(sai_edma_handle_t));
        }
    }

    int32_t RT1051DriverSAI::StartOutTransfer(const drivers::TransferParams &tx)
    {
        sai_transfer_t xfer = {0};
        sai_config_t config;
        sai_transfer_format_t sai_format = {0};

        txParams = tx;

        dmamux->Enable(parameters.txDMAChannel, txDMASource);
        dma->CreateHandle(parameters.txDMAChannel);

        // Initialize SAI Tx module
        SAI_TxGetDefaultConfig(&config);
        config.masterSlave =
            (parameters.masterslave == DriverSAIParams::MasterSlave ::Slave) ? kSAI_Slave : kSAI_Master;
        SAI_TxInit(base, &config);

        /* Configure the audio format */
        sai_format.bitWidth           = parameters.bitWidth;
        sai_format.channel            = 0U;
        sai_format.sampleRate_Hz      = parameters.sampleRate;
        sai_format.masterClockHz      = parameters.masterClock;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol           = static_cast<sai_protocol_t>(parameters.protocol);
        sai_format.stereo             = static_cast<sai_mono_stereo_t>(parameters.monostereo);
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferTxCreateHandleEDMA(base,
                                       &txHandle[enum_integer(instances)],
                                       txAudioCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(dma->GetHandle(parameters.txDMAChannel)));

        SAI_TransferTxSetFormatEDMA(
            base, &txHandle[enum_integer(instances)], &sai_format, parameters.masterClock, parameters.masterClock);

        /* Reset SAI Tx internal logic */
        SAI_TxSoftwareReset(base, kSAI_ResetTypeSoftware);

        xfer.data     = tx.data;
        xfer.dataSize = tx.size;
        SAI_TransferSendEDMA(base, &txHandle[enum_integer(instances)], &xfer);
    }

    int32_t RT1051DriverSAI::StopOutTransfer()
    {
        SAI_TxDisableInterrupts(base, kSAI_FIFOErrorInterruptEnable);
        if (txHandle[enum_integer(instances)].dmaHandle) {
            SAI_TransferTerminateSendEDMA(base, &txHandle[enum_integer(instances)]);
            memset(&txHandle[enum_integer(instances)], 0, sizeof(sai_edma_handle_t));
        }
    }

    int32_t RT1051DriverSAI::SetOutTransferEndCallback(std::function<int32_t()> callback)
    {
        txTransferEndCallback = callback;
    }

    int32_t RT1051DriverSAI::SetInTransferEndCallback(std::function<int32_t()> callback)
    {
        rxTransferEndCallback = callback;
    }

    void txAudioCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        sai_transfer_t xfer                 = {0};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        RT1051DriverSAI *inst               = (RT1051DriverSAI *)userData;

        if (inst->txTransferState == RT1051DriverSAI::TransferState::HalfTransfer) {

            xfer.dataSize = inst->txParams.size;
            xfer.data     = inst->txParams.data + (inst->txParams.size);
            SAI_TransferSendEDMA(base, &inst->txHandle[enum_integer(inst->instances)], &xfer);

            inst->txTransferEndCallback();

            inst->txTransferState = RT1051DriverSAI::TransferState::FullTransfer;
        }
        else {
            xfer.dataSize = inst->txParams.size;
            xfer.data     = inst->txParams.data;
            SAI_TransferSendEDMA(base, &inst->txHandle[enum_integer(inst->instances)], &xfer);

            inst->txTransferEndCallback();

            inst->txTransferState = RT1051DriverSAI::TransferState::HalfTransfer;
        }

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    void rxAudioCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        sai_transfer_t xfer                 = {0};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        RT1051DriverSAI *inst               = (RT1051DriverSAI *)userData;

        if (inst->txTransferState == RT1051DriverSAI::TransferState::HalfTransfer) {

            xfer.dataSize = inst->txParams.size;
            xfer.data     = inst->txParams.data + (inst->txParams.size);
            SAI_TransferReceiveEDMA(base, &inst->txHandle[enum_integer(inst->instances)], &xfer);

            inst->txTransferEndCallback();

            inst->txTransferState = RT1051DriverSAI::TransferState::FullTransfer;
        }
        else {
            xfer.dataSize = inst->txParams.size;
            xfer.data     = inst->txParams.data;
            SAI_TransferReceiveEDMA(base, &inst->txHandle[enum_integer(inst->instances)], &xfer);

            inst->txTransferEndCallback();

            inst->txTransferState = RT1051DriverSAI::TransferState::HalfTransfer;
        }

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
} // namespace drivers