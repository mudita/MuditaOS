// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-bsp/WorkerQueue.hpp>
#include <Audio/AbstractStream.hpp>
#include <Audio/AudioDevice.hpp>
#include <Audio/AudioFormat.hpp>
#include <Audio/codec.hpp>
#include "PulseAudioWrapper.hpp"

#include <variant>

namespace audio
{
    class LinuxAudioDevice : public audio::AudioDevice
    {
      public:
        explicit LinuxAudioDevice(const float initialVolume);

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
        using AudioProxy = WorkerQueue<std::size_t>;
        void scaleVolume(audio::AbstractStream::Span data);

        constexpr static std::initializer_list<unsigned int> supportedSampleRates  = {44100, 48000};
        constexpr static std::initializer_list<unsigned int> supportedBitWidths    = {16};
        constexpr static std::initializer_list<unsigned int> supportedChannelModes = {1, 2};

        std::vector<audio::AudioFormat> supportedFormats;
        audio::AudioFormat currentFormat;

        float volumeFactor = 1.0f;

        AudioProxy audioProxy;
        pulse_audio::Stream *stream{nullptr};
        std::size_t requestedBytes{};
    };

} // namespace audio
