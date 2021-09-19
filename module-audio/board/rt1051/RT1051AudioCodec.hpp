// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SAIAudioDevice.hpp"
#include "fsl_sai_edma.h"

#include "FreeRTOS.h"
#include "task.h"
#include <board/macros.h>

#include "board/rt1051/bsp/audio/CodecMAX98090.hpp"

#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"

#include <Audio/codec.hpp>

#include <initializer_list>
#include <vector>

namespace audio
{

    void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
    void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

    class RT1051AudioCodec : public SAIAudioDevice
    {

      public:
        friend void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

        RT1051AudioCodec(const audio::codec::Configuration &format);
        virtual ~RT1051AudioCodec();

        AudioDevice::RetCode Start() override final;
        AudioDevice::RetCode Stop() override final;
        AudioDevice::RetCode setOutputVolume(float vol) override final;
        AudioDevice::RetCode setInputGain(float gain) override final;
        auto getSupportedFormats() -> std::vector<AudioFormat> override final;
        auto getTraits() const -> Traits override final;
        auto getSourceFormat() -> AudioFormat override final;

        AudioDevice::RetCode OutputPathCtrl(audio::codec::OutputPath outputPath);
        AudioDevice::RetCode InputPathCtrl(audio::codec::InputPath inputPath);

      private:
        constexpr static TickType_t codecSettleTime                               = 20 * portTICK_PERIOD_MS;
        constexpr static std::initializer_list<unsigned int> supportedSampleRates = {
            8000, 16000, 32000, 44100, 48000, 96000};
        constexpr static std::initializer_list<unsigned int> supportedBitWidths    = {16};
        constexpr static std::initializer_list<unsigned int> supportedChannelModes = {1, 2};

        enum class State
        {
            Running,
            Stopped
        };

        struct SAIFormat
        {
            uint32_t sampleRate_Hz;   /*!< Sample rate of audio data */
            uint32_t bitWidth;        /*!< Data length of audio data, usually 8/16/24/32 bits */
            sai_mono_stereo_t stereo; /*!< Mono or stereo */
        };

        State state = State::Stopped;
        SAIFormat saiInFormat;
        SAIFormat saiOutFormat;
        CodecParamsMAX98090 codecParams;
        CodecMAX98090 codec;
        std::vector<audio::AudioFormat> formats;
        audio::codec::Configuration currentFormat;

        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle);
        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t rxHandle);

        void InitBsp();
        void DeinitBsp();
        void OutStart();
        void InStart();
        void OutStop();
        void InStop();
        void SetupEQ();

        CodecParamsMAX98090::InputPath getCodecInputPath(const audio::codec::Configuration &format);
        CodecParamsMAX98090::OutputPath getCodecOutputPath(const audio::codec::Configuration &format);
    };
} // namespace audio
