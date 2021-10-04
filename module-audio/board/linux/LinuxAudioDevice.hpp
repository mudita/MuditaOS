// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioDevice.hpp>
#include <Audio/AudioFormat.hpp>
#include <Audio/codec.hpp>

#include <portaudio.h>

#include <deque>

namespace audio
{
    class LinuxAudioDevice : public audio::AudioDevice
    {
      public:
        LinuxAudioDevice(const float initialVolume);
        virtual ~LinuxAudioDevice();

        auto Start() -> RetCode override;
        auto Stop() -> RetCode override;

        /// Set device output volume
        /// @param vol desired volume from 0 to 10
        /// @return RetCode::Success if OK, or RetCode::Failure otherwise
        auto setOutputVolume(float vol) -> RetCode override;

        /// Set device input gain
        /// @param gain desired input gain from 0 to 100
        /// @return RetCode::Success if OK, or RetCode::Failure otherwise
        auto setInputGain(float gain) -> RetCode override;

        auto getTraits() const -> Traits override;
        auto getSupportedFormats() -> std::vector<audio::AudioFormat> override;
        auto getSourceFormat() -> audio::AudioFormat override;

        // Endpoint control methods
        void onDataSend() override;
        void onDataReceive() override;
        void enableInput() override;
        void enableOutput() override;
        void disableInput() override;
        void disableOutput() override;

      private:
        int streamCallback(const void *input,
                           void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo *timeInfo,
                           PaStreamCallbackFlags statusFlags);
        bool isCacheReady(std::size_t expectedSize) const noexcept;
        void cacheToOutputBuffer(std::int16_t *buffer, std::size_t size);

        void closeStream();

        constexpr static std::initializer_list<unsigned int> supportedSampleRates  = {44100, 48000};
        constexpr static std::initializer_list<unsigned int> supportedBitWidths    = {16};
        constexpr static std::initializer_list<unsigned int> supportedChannelModes = {1, 2};

        std::vector<audio::AudioFormat> supportedFormats;
        audio::AudioFormat currentFormat;

        /// pointer to portaudio stream
        PaStream *stream = nullptr;

        /// Local cache to store data read from Pure stream
        std::deque<std::int16_t> cache;

        float volumeFactor = 1.0f;
    };

} // namespace audio
