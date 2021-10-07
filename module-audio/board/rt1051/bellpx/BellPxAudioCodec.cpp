// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellPxAudioCodec.hpp"
#include "board.h"
#include "dma_config.h"
#include <log/log.hpp>

#include "board/BoardDefinitions.hpp"
#include "board/rt1051/common/audio.hpp"

using audio::codec::Configuration;

namespace audio
{
    sai_edma_handle_t BellPxAudioCodec::txHandle = {};
    sai_edma_handle_t BellPxAudioCodec::rxHandle = {};

    BellPxAudioCodec::BellPxAudioCodec(const Configuration &format)
        : SAIAudioDevice(BELL_AUDIOCODEC_SAIx, &rxHandle, &txHandle), saiInFormat{}, saiOutFormat{},
          codecParams{}, codec{},
          formats(audio::AudioFormat::makeMatrix(supportedSampleRates, supportedBitWidths, supportedChannelModes)),
          currentFormat(format)
    {}

    BellPxAudioCodec::~BellPxAudioCodec()
    {
        Stop();
        DeinitBsp();
    }

    AudioDevice::RetCode BellPxAudioCodec::Start()
    {
        if (state == State::Running) {
            return AudioDevice::RetCode::Failure;
        }

        auto sampleRate = CodecParams::ValToSampleRate(currentFormat.sampleRate_Hz);
        if (sampleRate == CodecParams::SampleRate::Invalid) {
            LOG_ERROR("Unsupported sample rate");
            return AudioDevice::RetCode::Failure;
        };

        InitBsp();

        saiInFormat.bitWidth      = currentFormat.bitWidth;
        saiInFormat.sampleRate_Hz = currentFormat.sampleRate_Hz;

        saiOutFormat.bitWidth      = currentFormat.bitWidth;
        saiOutFormat.sampleRate_Hz = currentFormat.sampleRate_Hz;

        if (currentFormat.flags & static_cast<uint32_t>(audio::codec::Flags::InputLeft)) {
            saiInFormat.stereo = kSAI_MonoLeft;
            InStart();
        }
        else if (currentFormat.flags & static_cast<uint32_t>(audio::codec::Flags::InputRight)) {
            saiInFormat.stereo = kSAI_MonoRight;
            InStart();
        }
        else if (currentFormat.flags & static_cast<uint32_t>(audio::codec::Flags::InputStereo)) {
            saiInFormat.stereo = kSAI_Stereo;
            InStart();
        }

        if (currentFormat.flags & static_cast<uint32_t>(audio::codec::Flags::OutputMono)) {
            saiOutFormat.stereo = kSAI_MonoLeft;
            OutStart();
        }
        else if (currentFormat.flags & static_cast<uint32_t>(audio::codec::Flags::OutputStereo)) {
            saiOutFormat.stereo = kSAI_Stereo;
            OutStart();
        }

        codecParams.sampleRate = sampleRate;
        codecParams.outVolume  = currentFormat.outputVolume;
        codecParams.inGain     = currentFormat.inputGain;

        txEnabled = true;
        initiateTxTransfer();
        codec.Start(codecParams);

        state = State::Running;

        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode BellPxAudioCodec::Stop()
    {
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

    AudioDevice::RetCode BellPxAudioCodec::setOutputVolume(float vol)
    {
        currentFormat.outputVolume = vol;
        CodecParams params;
        params.outVolume = vol;
        params.opCmd     = CodecParams::Cmd::SetOutVolume;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode BellPxAudioCodec::setInputGain(float gain)
    {
        currentFormat.inputGain = gain;
        CodecParams params;
        params.inGain = gain;
        params.opCmd  = CodecParams::Cmd::SetInGain;
        codec.Ioctrl(params);
        return AudioDevice::RetCode::Success;
    }

    void BellPxAudioCodec::InitBsp()
    {
        bsp::audio::init();
    }

    void BellPxAudioCodec::DeinitBsp()
    {
        bsp::audio::deinit();
    }

    void BellPxAudioCodec::InStart()
    {
        sai_transfer_format_t sai_format;
        auto audioCfg = bsp::audio::AudioConfig::get();

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

        SAI_TransferRxCreateHandleEDMA(BELL_AUDIOCODEC_SAIx,
                                       &rxHandle,
                                       rxAudioCodecCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(audioCfg->rxDMAHandle->GetHandle()));

        SAI_TransferRxSetFormatEDMA(
            BELL_AUDIOCODEC_SAIx, &rxHandle, &sai_format, audioCfg->mclkSourceClockHz, audioCfg->mclkSourceClockHz);

        DisableIRQ(BELL_AUDIOCODEC_SAIx_RX_IRQ);

        /* Reset SAI Rx internal logic */
        SAI_RxSoftwareReset(BELL_AUDIOCODEC_SAIx, kSAI_ResetTypeSoftware);
    }

    void BellPxAudioCodec::OutStart()
    {
        sai_transfer_format_t sai_format;
        auto audioCfg = bsp::audio::AudioConfig::get();

        /* Configure the audio format */
        sai_format.bitWidth           = saiOutFormat.bitWidth;
        sai_format.channel            = 0U;
        sai_format.sampleRate_Hz      = saiOutFormat.sampleRate_Hz;
        sai_format.masterClockHz      = audioCfg->mclkSourceClockHz;
        sai_format.isFrameSyncCompact = true;
        sai_format.protocol           = audioCfg->config.protocol;
        sai_format.stereo             = saiOutFormat.stereo;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
        sai_format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

        SAI_TransferTxCreateHandleEDMA(BELL_AUDIOCODEC_SAIx,
                                       &txHandle,
                                       txAudioCodecCallback,
                                       this,
                                       reinterpret_cast<edma_handle_t *>(audioCfg->txDMAHandle->GetHandle()));
        SAI_TransferTxSetFormatEDMA(
            BELL_AUDIOCODEC_SAIx, &txHandle, &sai_format, audioCfg->mclkSourceClockHz, audioCfg->mclkSourceClockHz);

        SAI_TxSetBitClockPolarity(BELL_AUDIOCODEC_SAIx, kSAI_PolarityActiveLow);

        DisableIRQ(BELL_AUDIOCODEC_SAIx_TX_IRQ);

        /* Reset SAI Tx internal logic */
        SAI_TxSoftwareReset(BELL_AUDIOCODEC_SAIx, kSAI_ResetTypeSoftware);
    }

    void BellPxAudioCodec::OutStop()
    {
        SAI_TxDisableInterrupts(BELL_AUDIOCODEC_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (txHandle.dmaHandle) {
            SAI_TransferTerminateSendEDMA(BELL_AUDIOCODEC_SAIx, &txHandle);
        }
        memset(&txHandle, 0, sizeof(txHandle));
    }

    void BellPxAudioCodec::InStop()
    {
        SAI_RxDisableInterrupts(BELL_AUDIOCODEC_SAIx, kSAI_FIFOErrorInterruptEnable);
        if (rxHandle.dmaHandle) {
            SAI_TransferAbortReceiveEDMA(BELL_AUDIOCODEC_SAIx, &rxHandle);
        }
        memset(&rxHandle, 0, sizeof(rxHandle));
    }

    auto BellPxAudioCodec::getSupportedFormats() -> std::vector<AudioFormat>
    {
        return formats;
    }

    auto BellPxAudioCodec::getTraits() const -> Traits
    {
        return Traits{.usesDMA = true};
    }

    auto BellPxAudioCodec::getSourceFormat() -> audio::AudioFormat
    {
        if (currentFormat.flags == 0) {
            return audio::nullFormat;
        }

        auto isMono = (currentFormat.flags & static_cast<unsigned int>(audio::codec::Flags::InputStereo)) == 0;
        return audio::AudioFormat{currentFormat.sampleRate_Hz, currentFormat.bitWidth, isMono ? 1U : 2U};
    }

    void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        auto self = static_cast<BellPxAudioCodec *>(userData);
        self->onDataReceive();
    }

    void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
    {
        auto self = static_cast<BellPxAudioCodec *>(userData);
        self->onDataSend();
    }

} // namespace audio
