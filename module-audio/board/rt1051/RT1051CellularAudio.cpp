// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051CellularAudio.hpp"
#include <board/board.h>
#include "dma_config.h"
#include <log.hpp>

#include "board/BoardDefinitions.hpp"

namespace audio
{

    using namespace drivers;

    sai_edma_handle_t RT1051CellularAudio::txHandle = {};
    sai_edma_handle_t RT1051CellularAudio::rxHandle = {};

    RT1051CellularAudio::RT1051CellularAudio() : SAIAudioDevice(BOARD_CELLULAR_AUDIO_SAIx, &rxHandle, &txHandle)
    {}

    RT1051CellularAudio::~RT1051CellularAudio()
    {
        Stop();
        Deinit();
    }

    AudioDevice::RetCode RT1051CellularAudio::Start()
    {
        if (state == State::Running) {
            LOG_ERROR("Audio device already running");
            return AudioDevice::RetCode::Failure;
        }

        Init();
        InStart();
        OutStart();

        state = State::Running;

        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051CellularAudio::Stop()
    {
        if (state == State::Stopped) {
            return AudioDevice::RetCode::Failure;
        }

        InStop();
        OutStop();

        state = State::Stopped;

        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051CellularAudio::setOutputVolume(float vol)
    {
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051CellularAudio::setInputGain(float gain)
    {
        return AudioDevice::RetCode::Success;
    }

    void RT1051CellularAudio::Init()
    {

        pll = DriverPLL::Create(static_cast<PLLInstances>(BoardDefinitions::AUDIO_PLL), DriverPLLParams{});

        dmamux = DriverDMAMux::Create(static_cast<DMAMuxInstances>(BoardDefinitions::CELLULAR_AUDIO_DMAMUX),
                                      DriverDMAMuxParams{});

        dma = DriverDMA::Create(static_cast<DMAInstances>(BoardDefinitions::CELLULAR_AUDIO_DMA), DriverDMAParams{});

        // Enable MCLK clock
        IOMUXC_GPR->GPR1 |= BOARD_CELLULAR_AUDIO_SAIx_MCLK_MASK;

        txDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::CELLULAR_AUDIO_TX_DMA_CHANNEL));
        rxDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::CELLULAR_AUDIO_RX_DMA_CHANNEL));
        dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_AUDIO_TX_DMA_CHANNEL),
                       BSP_CELLULAR_AUDIO_SAIx_DMA_TX_SOURCE);
        dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_AUDIO_RX_DMA_CHANNEL),
                       BSP_CELLULAR_AUDIO_SAIx_DMA_RX_SOURCE);

        mclkSourceClockHz = GetPerphSourceClock(PerphClock_SAI1);

        // Initialize SAI Tx module
        SAI_TxGetDefaultConfig(&config);
        config.masterSlave = kSAI_Slave;
        config.protocol    = kSAI_BusPCMA;
        SAI_TxInit(BOARD_CELLULAR_AUDIO_SAIx, &config);

        // Initialize SAI Rx module
        SAI_RxGetDefaultConfig(&config);
        config.masterSlave = kSAI_Slave;
        config.protocol    = kSAI_BusPCMA;
        SAI_RxInit(BOARD_CELLULAR_AUDIO_SAIx, &config);
    }

    void RT1051CellularAudio::Deinit()
    {
        memset(&config, 0, sizeof config);
        SAI_Deinit(BOARD_CELLULAR_AUDIO_SAIx);
        if (dmamux) {
            dmamux->Disable(static_cast<uint32_t>(BoardDefinitions ::CELLULAR_AUDIO_TX_DMA_CHANNEL));
            dmamux->Disable(static_cast<uint32_t>(BoardDefinitions ::CELLULAR_AUDIO_RX_DMA_CHANNEL));
        }
    }

    void RT1051CellularAudio::InStart()
    {
        sai_transfer_format_t sai_format = {};

        /* Configure the audio format */
        sai_format.bitWidth           = supportedBitWidth;
        sai_format.channel            = 0U;
        sai_format.sampleRate_Hz      = supportedSampleRate;
        sai_format.masterClockHz      = mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol           = config.protocol;
        sai_format.stereo             = kSAI_MonoLeft;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferRxCreateHandleEDMA(BOARD_CELLULAR_AUDIO_SAIx,
                                       &rxHandle,
                                       rxCellularCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(rxDMAHandle->GetHandle()));

        SAI_TransferRxSetFormatEDMA(
            BOARD_CELLULAR_AUDIO_SAIx, &rxHandle, &sai_format, mclkSourceClockHz, mclkSourceClockHz);

        DisableIRQ(BOARD_CELLULAR_AUDIO_SAIx_RX_IRQ);

        /* Reset SAI Rx internal logic */
        SAI_RxSoftwareReset(BOARD_CELLULAR_AUDIO_SAIx, kSAI_ResetTypeSoftware);
    }

    void RT1051CellularAudio::OutStart()
    {
        sai_transfer_format_t sai_format;

        /* Configure the audio format */
        sai_format.bitWidth           = supportedBitWidth;
        sai_format.channel            = 0U;
        sai_format.sampleRate_Hz      = supportedSampleRate;
        sai_format.masterClockHz      = mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol           = config.protocol;
        sai_format.stereo             = kSAI_MonoLeft;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferTxCreateHandleEDMA(BOARD_CELLULAR_AUDIO_SAIx,
                                       &txHandle,
                                       txCellularCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(txDMAHandle->GetHandle()));
        SAI_TransferTxSetFormatEDMA(
            BOARD_CELLULAR_AUDIO_SAIx, &txHandle, &sai_format, mclkSourceClockHz, mclkSourceClockHz);

        DisableIRQ(BOARD_CELLULAR_AUDIO_SAIx_TX_IRQ);

        /* Reset SAI Tx internal logic */
        SAI_TxSoftwareReset(BOARD_CELLULAR_AUDIO_SAIx, kSAI_ResetTypeSoftware);
    }

    void RT1051CellularAudio::OutStop()
    {
        SAI_TxDisableInterrupts(BOARD_CELLULAR_AUDIO_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (txHandle.dmaHandle) {
            SAI_TransferTerminateSendEDMA(BOARD_CELLULAR_AUDIO_SAIx, &txHandle);
        }
        memset(&txHandle, 0, sizeof(txHandle));
    }

    void RT1051CellularAudio::InStop()
    {
        SAI_RxDisableInterrupts(BOARD_CELLULAR_AUDIO_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (rxHandle.dmaHandle) {
            SAI_TransferAbortReceiveEDMA(BOARD_CELLULAR_AUDIO_SAIx, &rxHandle);
        }
        memset(&rxHandle, 0, sizeof(rxHandle));
    }

    auto RT1051CellularAudio::getSupportedFormats() -> std::vector<AudioFormat>
    {
        return std::vector<AudioFormat>{getSourceFormat()};
    }

    auto RT1051CellularAudio::getSourceFormat() -> AudioFormat
    {
        return AudioFormat(supportedSampleRate, supportedBitWidth, supportedChannels);
    }

    auto RT1051CellularAudio::getTraits() const -> Traits
    {
        return Traits{.usesDMA = true};
    }

    void rxCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        auto self = static_cast<RT1051CellularAudio *>(userData);

        self->onDataReceive();
    }

    void txCellularCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        auto self = static_cast<RT1051CellularAudio *>(userData);

        self->onDataSend();
    }

} // namespace audio
