// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051Audiocodec.hpp"
#include "board.h"
#include "dma_config.h"
#include "log/log.hpp"

#include "bsp/BoardDefinitions.hpp"

#include <mutex.hpp>

namespace bsp
{

    using namespace drivers;

    std::shared_ptr<drivers::DriverPLL> RT1051Audiocodec::pllAudio;
    std::shared_ptr<drivers::DriverDMAMux> RT1051Audiocodec::dmamux;
    std::shared_ptr<drivers::DriverDMA> RT1051Audiocodec::dma;
    std::unique_ptr<drivers::DriverDMAHandle> RT1051Audiocodec::rxDMAHandle;
    std::unique_ptr<drivers::DriverDMAHandle> RT1051Audiocodec::txDMAHandle;
    sai_config_t RT1051Audiocodec::config             = {};
    std::uint32_t RT1051Audiocodec::mclkSourceClockHz = 0;
    sai_edma_handle_t RT1051Audiocodec::txHandle      = {};
    sai_edma_handle_t RT1051Audiocodec::rxHandle      = {};

    RT1051Audiocodec::RT1051Audiocodec(bsp::AudioDevice::audioCallback_t callback)
        : AudioDevice(callback), saiInFormat{}, saiOutFormat{}, codecParams{}, codec{}
    {
        isInitialized = true;
    }

    RT1051Audiocodec::~RT1051Audiocodec()
    {
        Stop();
    }

    AudioDevice::RetCode RT1051Audiocodec::Start(const bsp::AudioDevice::Format &format)
    {
        cpp_freertos::LockGuard lock(mutex);

        if (state == State::Running) {
            return AudioDevice::RetCode::Failure;
        }

        saiInFormat.bitWidth      = format.bitWidth;
        saiInFormat.sampleRate_Hz = format.sampleRate_Hz;

        saiOutFormat.bitWidth      = format.bitWidth;
        saiOutFormat.sampleRate_Hz = format.sampleRate_Hz;

        if (format.flags & static_cast<uint32_t>(AudioDevice::Flags::InputLeft)) {
            saiInFormat.stereo = kSAI_MonoLeft;
            InStart();
        }
        else if (format.flags & static_cast<uint32_t>(AudioDevice::Flags::InputRight)) {
            saiInFormat.stereo = kSAI_MonoRight;
            InStart();
        }
        else if (format.flags & static_cast<uint32_t>(AudioDevice::Flags::InputStereo)) {
            saiInFormat.stereo = kSAI_Stereo;
            InStart();
        }

        if (format.flags & static_cast<uint32_t>(AudioDevice::Flags::OutputMono)) {
            saiOutFormat.stereo = kSAI_MonoLeft;
            OutStart();
        }
        else if (format.flags & static_cast<uint32_t>(AudioDevice::Flags::OutputStereo)) {
            saiOutFormat.stereo = kSAI_Stereo;
            OutStart();
        }

        codecParams.sampleRate = CodecParamsMAX98090::ValToSampleRate(format.sampleRate_Hz);
        if (codecParams.sampleRate == CodecParamsMAX98090::SampleRate::Invalid) {
            LOG_ERROR("Unsupported sample rate");
        }

        codecParams.inputPath  = format.inputPath;
        codecParams.outputPath = format.outputPath;
        codecParams.outVolume  = format.outputVolume;
        codecParams.inGain     = format.inputGain;
        codec.Start(codecParams);

        // Store format passed
        currentFormat = format;

        state = State::Running;

        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051Audiocodec::Stop()
    {
        cpp_freertos::LockGuard lock(mutex);

        if (state == State::Stopped) {
            return AudioDevice::RetCode::Failure;
        }

        codec.Stop();

        InStop();
        OutStop();

        state = State::Stopped;
        vTaskDelay(codecSettleTime);

        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051Audiocodec::OutputVolumeCtrl(float vol)
    {
        currentFormat.outputVolume = vol;
        CodecParamsMAX98090 params;
        params.outVolume = vol;
        params.opCmd     = CodecParamsMAX98090::Cmd::SetOutVolume;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051Audiocodec::InputGainCtrl(float gain)
    {
        currentFormat.inputGain = gain;
        CodecParamsMAX98090 params;
        params.inGain = gain;
        params.opCmd  = CodecParamsMAX98090::Cmd::SetInGain;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051Audiocodec::InputPathCtrl(InputPath inputPath)
    {
        currentFormat.inputPath = inputPath;
        CodecParamsMAX98090 params;
        params.inputPath = inputPath;
        params.opCmd     = CodecParamsMAX98090::Cmd::SetInput;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051Audiocodec::OutputPathCtrl(OutputPath outputPath)
    {
        currentFormat.outputPath = outputPath;
        CodecParamsMAX98090 params;
        params.outputPath = outputPath;
        params.opCmd      = CodecParamsMAX98090::Cmd::SetOutput;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    bool RT1051Audiocodec::IsFormatSupported(const bsp::AudioDevice::Format &format)
    {

        if (CodecParamsMAX98090::ValToSampleRate(format.sampleRate_Hz) == CodecParamsMAX98090::SampleRate::Invalid) {
            return false;
        }
        return true;
    }
    // INTERNALS

    void RT1051Audiocodec::Init()
    {

        pllAudio = DriverPLL::Create(static_cast<PLLInstances>(BoardDefinitions ::AUDIO_PLL), DriverPLLParams{});
        dmamux   = DriverDMAMux::Create(static_cast<DMAMuxInstances>(BoardDefinitions ::AUDIOCODEC_DMAMUX),
                                      DriverDMAMuxParams{});
        dma      = DriverDMA::Create(static_cast<DMAInstances>(BoardDefinitions ::AUDIOCODEC_DMA), DriverDMAParams{});

        // Enable MCLK clock
        IOMUXC_GPR->GPR1 |= BOARD_AUDIOCODEC_SAIx_MCLK_MASK;

        txDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions ::AUDIOCODEC_TX_DMA_CHANNEL));
        rxDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions ::AUDIOCODEC_RX_DMA_CHANNEL));
        dmamux->Enable(static_cast<uint32_t>(BoardDefinitions ::AUDIOCODEC_TX_DMA_CHANNEL),
                       BSP_AUDIOCODEC_SAIx_DMA_TX_SOURCE);
        dmamux->Enable(static_cast<uint32_t>(BoardDefinitions ::AUDIOCODEC_RX_DMA_CHANNEL),
                       BSP_AUDIOCODEC_SAIx_DMA_RX_SOURCE);

        mclkSourceClockHz = GetPerphSourceClock(PerphClock_SAI2);

        // Initialize SAI Tx module
        SAI_TxGetDefaultConfig(&config);
        config.masterSlave = kSAI_Slave;
        SAI_TxInit(BOARD_AUDIOCODEC_SAIx, &config);

        // Initialize SAI Rx module
        SAI_RxGetDefaultConfig(&config);

        config.masterSlave = kSAI_Slave;
        SAI_RxInit(BOARD_AUDIOCODEC_SAIx, &config);
    }

    void RT1051Audiocodec::Deinit()
    {
        memset(&config, 0, sizeof config);
        SAI_Deinit(BOARD_AUDIOCODEC_SAIx);
        if (dmamux) {
            dmamux->Disable(static_cast<uint32_t>(BoardDefinitions ::AUDIOCODEC_TX_DMA_CHANNEL));
            dmamux->Disable(static_cast<uint32_t>(BoardDefinitions ::AUDIOCODEC_RX_DMA_CHANNEL));
        }

        // force order of destruction
        txDMAHandle.reset();
        rxDMAHandle.reset();
        dma.reset();
        dmamux.reset();
        pllAudio.reset();
    }

    void RT1051Audiocodec::InStart()
    {
        sai_transfer_format_t sai_format = {};

        /* Configure the audio format */
        sai_format.bitWidth           = saiInFormat.bitWidth;
        sai_format.channel            = 0U;
        sai_format.sampleRate_Hz      = saiInFormat.sampleRate_Hz;
        sai_format.masterClockHz      = mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol           = config.protocol;
        sai_format.stereo             = saiInFormat.stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferRxCreateHandleEDMA(BOARD_AUDIOCODEC_SAIx,
                                       &rxHandle,
                                       rxAudioCodecCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(rxDMAHandle->GetHandle()));

        SAI_TransferRxSetFormatEDMA(
            BOARD_AUDIOCODEC_SAIx, &rxHandle, &sai_format, mclkSourceClockHz, mclkSourceClockHz);

        DisableIRQ(BOARD_AUDIOCODEC_SAIx_RX_IRQ);

        /* Reset SAI Rx internal logic */
        SAI_RxSoftwareReset(BOARD_AUDIOCODEC_SAIx, kSAI_ResetTypeSoftware);

        if (!source.isConnected()) {
            LOG_FATAL("No output stream connected!");
            return;
        }

        /// initiate first read
        audio::Stream::Span dataSpan;
        source.getStream()->reserve(dataSpan);
        auto xfer = sai_transfer_t{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
        SAI_TransferReceiveEDMA(BOARD_AUDIOCODEC_SAIx, &rxHandle, &xfer);
    }

    void RT1051Audiocodec::OutStart()
    {
        sai_transfer_format_t sai_format = {};

        /* Configure the audio format */
        sai_format.bitWidth           = saiOutFormat.bitWidth;
        sai_format.channel            = 0U;
        sai_format.sampleRate_Hz      = saiOutFormat.sampleRate_Hz;
        sai_format.masterClockHz      = mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol           = config.protocol;
        sai_format.stereo             = saiOutFormat.stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferTxCreateHandleEDMA(BOARD_AUDIOCODEC_SAIx,
                                       &txHandle,
                                       txAudioCodecCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(txDMAHandle->GetHandle()));
        SAI_TransferTxSetFormatEDMA(
            BOARD_AUDIOCODEC_SAIx, &txHandle, &sai_format, mclkSourceClockHz, mclkSourceClockHz);

        DisableIRQ(BOARD_AUDIOCODEC_SAIx_TX_IRQ);

        /* Reset SAI Tx internal logic */
        SAI_TxSoftwareReset(BOARD_AUDIOCODEC_SAIx, kSAI_ResetTypeSoftware);

        if (!sink.isConnected()) {
            LOG_FATAL("No input stream connected!");
            return;
        }

        auto nullSpan = sink.getStream()->getNullSpan();
        auto xfer     = sai_transfer_t{.data = nullSpan.data, .dataSize = nullSpan.dataSize};
        SAI_TransferSendEDMA(BOARD_AUDIOCODEC_SAIx, &txHandle, &xfer);
    }

    void RT1051Audiocodec::OutStop()
    {
        SAI_TxDisableInterrupts(BOARD_AUDIOCODEC_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (txHandle.dmaHandle) {
            SAI_TransferTerminateSendEDMA(BOARD_AUDIOCODEC_SAIx, &txHandle);
        }
        memset(&txHandle, 0, sizeof(txHandle));
        if (sink.isConnected()) {
            sink.getStream()->unpeek();
        }
    }

    void RT1051Audiocodec::InStop()
    {
        SAI_RxDisableInterrupts(BOARD_AUDIOCODEC_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (rxHandle.dmaHandle) {
            SAI_TransferAbortReceiveEDMA(BOARD_AUDIOCODEC_SAIx, &rxHandle);
        }
        memset(&rxHandle, 0, sizeof(rxHandle));
        if (source.isConnected()) {
            source.getStream()->release();
        }
    }

    void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        audio::Stream::Span dataSpan;
        auto self    = static_cast<RT1051Audiocodec *>(userData);
        auto &source = self->source;

        /// exit if not connected to the stream
        if (!source.isConnected()) {
            return;
        }

        /// reserve space for the next read commiting previously reserved block before
        source.getStream()->commit();
        source.getStream()->reserve(dataSpan);

        sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
        SAI_TransferReceiveEDMA(BOARD_AUDIOCODEC_SAIx, &self->rxHandle, &xfer);
    }

    void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        audio::Stream::Span dataSpan;
        auto self  = static_cast<RT1051Audiocodec *>(userData);
        auto &sink = self->sink;

        /// exit if not connected to the stream
        if (!sink.isConnected()) {
            return;
        }

        /// pop previous read and peek next
        sink.getStream()->consume();
        sink.getStream()->peek(dataSpan);

        sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
        SAI_TransferSendEDMA(BOARD_AUDIOCODEC_SAIx, &self->txHandle, &xfer);
    }

} // namespace bsp
