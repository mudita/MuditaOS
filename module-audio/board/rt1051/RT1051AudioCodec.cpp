// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051AudioCodec.hpp"
#include "board.h"
#include "dma_config.h"
#include "log/log.hpp"

#include "bsp/BoardDefinitions.hpp"
#include "board/rt1051/common/audio.hpp"

#include <mutex.hpp>

namespace audio
{
    sai_edma_handle_t RT1051AudioCodec::txHandle = {};
    sai_edma_handle_t RT1051AudioCodec::rxHandle = {};

    RT1051AudioCodec::RT1051AudioCodec()
        : SAIAudioDevice(BOARD_AUDIOCODEC_SAIx, &rxHandle, &txHandle), saiInFormat{}, saiOutFormat{},
          codecParams{}, codec{}
    {
        isInitialized = true;
    }

    RT1051AudioCodec::~RT1051AudioCodec()
    {
        Stop();
    }

    CodecParamsMAX98090::InputPath RT1051AudioCodec::getCodecInputPath(const AudioDevice::Format &format)
    {
        switch (format.inputPath) {
        case AudioDevice::InputPath::Headphones:
            return CodecParamsMAX98090::InputPath::Headphones;

        case AudioDevice::InputPath::Microphone:
            return CodecParamsMAX98090::InputPath::Microphone;

        default:
            return CodecParamsMAX98090::InputPath::None;
        };
    }

    CodecParamsMAX98090::OutputPath RT1051AudioCodec::getCodecOutputPath(const AudioDevice::Format &format)
    {
        auto mono = (format.flags & static_cast<std::uint32_t>(AudioDevice::Flags::OutputMono)) != 0;

        switch (format.outputPath) {
        case AudioDevice::OutputPath::Headphones:
            return mono ? CodecParamsMAX98090::OutputPath::HeadphonesMono : CodecParamsMAX98090::OutputPath::Headphones;

        case AudioDevice::OutputPath::Earspeaker:
            return CodecParamsMAX98090::OutputPath::Earspeaker;

        case AudioDevice::OutputPath::Loudspeaker:
            return mono ? CodecParamsMAX98090::OutputPath::LoudspeakerMono
                        : CodecParamsMAX98090::OutputPath::Loudspeaker;

        default:
            return CodecParamsMAX98090::OutputPath::None;
        }
    }

    AudioDevice::RetCode RT1051AudioCodec::Start(const AudioDevice::Format &format)
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

        codecParams.inputPath  = getCodecInputPath(format);
        codecParams.outputPath = getCodecOutputPath(format);
        codecParams.outVolume  = format.outputVolume;
        codecParams.inGain     = format.inputGain;
        codec.Start(codecParams);

        // Store format passed
        currentFormat = format;

        state = State::Running;

        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051AudioCodec::Stop()
    {
        cpp_freertos::LockGuard lock(mutex);

        if (state == State::Stopped) {
            return AudioDevice::RetCode::Failure;
        }

        InStop();
        OutStop();

        codec.Stop();

        state = State::Stopped;
        vTaskDelay(codecSettleTime);

        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051AudioCodec::OutputVolumeCtrl(float vol)
    {
        currentFormat.outputVolume = vol;
        CodecParamsMAX98090 params;
        params.outVolume = vol;
        params.opCmd     = CodecParamsMAX98090::Cmd::SetOutVolume;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051AudioCodec::InputGainCtrl(float gain)
    {
        currentFormat.inputGain = gain;
        CodecParamsMAX98090 params;
        params.inGain = gain;
        params.opCmd  = CodecParamsMAX98090::Cmd::SetInGain;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051AudioCodec::InputPathCtrl(InputPath inputPath)
    {
        currentFormat.inputPath = inputPath;
        CodecParamsMAX98090 params;
        params.inputPath = getCodecInputPath(currentFormat);
        params.opCmd     = CodecParamsMAX98090::Cmd::SetInput;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode RT1051AudioCodec::OutputPathCtrl(OutputPath outputPath)
    {
        currentFormat.outputPath = outputPath;
        CodecParamsMAX98090 params;
        params.outputPath = getCodecOutputPath(currentFormat);
        params.opCmd      = CodecParamsMAX98090::Cmd::SetOutput;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    bool RT1051AudioCodec::IsFormatSupported(const AudioDevice::Format &format)
    {

        if (CodecParamsMAX98090::ValToSampleRate(format.sampleRate_Hz) == CodecParamsMAX98090::SampleRate::Invalid) {
            return false;
        }
        return true;
    }

    void RT1051AudioCodec::InStart()
    {
        sai_transfer_format_t sai_format;
        auto audioCfg = bsp::AudioConfig::get();

        /* Configure the audio format */
        sai_format.bitWidth           = saiInFormat.bitWidth;
        sai_format.channel            = 0U;
        sai_format.sampleRate_Hz      = saiInFormat.sampleRate_Hz;
        sai_format.masterClockHz      = audioCfg->mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol           = audioCfg->config.protocol;
        sai_format.stereo             = saiInFormat.stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferRxCreateHandleEDMA(BOARD_AUDIOCODEC_SAIx,
                                       &rxHandle,
                                       rxAudioCodecCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(audioCfg->rxDMAHandle->GetHandle()));

        SAI_TransferRxSetFormatEDMA(
            BOARD_AUDIOCODEC_SAIx, &rxHandle, &sai_format, audioCfg->mclkSourceClockHz, audioCfg->mclkSourceClockHz);

        DisableIRQ(BOARD_AUDIOCODEC_SAIx_RX_IRQ);

        /* Reset SAI Rx internal logic */
        SAI_RxSoftwareReset(BOARD_AUDIOCODEC_SAIx, kSAI_ResetTypeSoftware);
    }

    void RT1051AudioCodec::OutStart()
    {
        sai_transfer_format_t sai_format;
        auto audioCfg = bsp::AudioConfig::get();

        /* Configure the audio format */
        sai_format.bitWidth           = saiOutFormat.bitWidth;
        sai_format.channel            = 0U;
        sai_format.sampleRate_Hz      = saiOutFormat.sampleRate_Hz;
        sai_format.masterClockHz      = audioCfg->mclkSourceClockHz;
        sai_format.isFrameSyncCompact = false;
        sai_format.protocol           = audioCfg->config.protocol;
        sai_format.stereo             = saiOutFormat.stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferTxCreateHandleEDMA(BOARD_AUDIOCODEC_SAIx,
                                       &txHandle,
                                       txAudioCodecCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(audioCfg->txDMAHandle->GetHandle()));
        SAI_TransferTxSetFormatEDMA(
            BOARD_AUDIOCODEC_SAIx, &txHandle, &sai_format, audioCfg->mclkSourceClockHz, audioCfg->mclkSourceClockHz);

        DisableIRQ(BOARD_AUDIOCODEC_SAIx_TX_IRQ);

        /* Reset SAI Tx internal logic */
        SAI_TxSoftwareReset(BOARD_AUDIOCODEC_SAIx, kSAI_ResetTypeSoftware);
    }

    void RT1051AudioCodec::OutStop()
    {
        SAI_TxDisableInterrupts(BOARD_AUDIOCODEC_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (txHandle.dmaHandle) {
            SAI_TransferTerminateSendEDMA(BOARD_AUDIOCODEC_SAIx, &txHandle);
        }
        memset(&txHandle, 0, sizeof(txHandle));
    }

    void RT1051AudioCodec::InStop()
    {
        SAI_RxDisableInterrupts(BOARD_AUDIOCODEC_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (rxHandle.dmaHandle) {
            SAI_TransferAbortReceiveEDMA(BOARD_AUDIOCODEC_SAIx, &rxHandle);
        }
        memset(&rxHandle, 0, sizeof(rxHandle));
    }

    void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        auto self = static_cast<RT1051AudioCodec *>(userData);
        self->onDataReceive();
    }

    void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        auto self = static_cast<RT1051AudioCodec *>(userData);
        self->onDataSend();
    }

} // namespace audio
