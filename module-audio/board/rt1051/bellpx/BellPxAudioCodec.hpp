// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SAIAudioDevice.hpp"
#include "fsl_sai_edma.h"

#include "FreeRTOS.h"
#include "task.h"
#include "macros.h"

#include <bsp/audio/CodecAW8898.hpp>

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

    class BellPxAudioCodec : public SAIAudioDevice
    {

      public:
        friend void txAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        friend void rxAudioCodecCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

        BellPxAudioCodec(const audio::codec::Configuration &format);
        virtual ~BellPxAudioCodec();

        AudioDevice::RetCode Start() final;
        AudioDevice::RetCode Stop() final;
        AudioDevice::RetCode setInputGain(float gain) final;
        auto getSupportedFormats() -> std::vector<AudioFormat> final;
        auto getTraits() const -> Traits final;
        auto getSourceFormat() -> AudioFormat final;

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
        CodecParams codecParams;
        CodecAW8898 codec;
        std::vector<audio::AudioFormat> formats;
        audio::codec::Configuration currentFormat;

        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle);
        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t rxHandle);

        void OutStart();
        void InStart();
        void OutStop();
        void InStop();
    };
} // namespace audio
