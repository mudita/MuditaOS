// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LinuxAudioDevice.hpp"
#include <Audio/Stream.hpp>
#include <log/log.hpp>
#include <cmath>
#include <stdexcept>

namespace
{
    /// Creating and destroying context each time LinuxAudioDevice is spawned seems very unstable. It causes various
    /// problems, from raising sanitizer errors to completely freezing the simulator. As a solution, another approach
    /// has been chosen. Instead of recreating the pulse audio context, we rely on static instance of it, which is valid
    /// throughout the life of the simulator process. During the standard operation, we only create and attach audio
    /// streams, which have proven to be a fast and reliable way of doing things. All the necessary context cleaning is
    /// handled in the context's destructor during the simulator process close.
    static audio::pulse_audio::Context ctx;
    audio::pulse_audio::Context &get_context()
    {
        return ctx;
    }
} // namespace

namespace audio
{
    LinuxAudioDevice::LinuxAudioDevice(const float initialVolume)
        : supportedFormats(
              audio::AudioFormat::makeMatrix(supportedSampleRates, supportedBitWidths, supportedChannelModes)),
          audioProxy("audioProxy", [this](const auto &data) { onDataSend(); })
    {
        setOutputVolume(initialVolume);
    }

    auto LinuxAudioDevice::Start() -> RetCode
    {
        if (!isSinkConnected()) {
            return AudioDevice::RetCode::Failure;
        }
        return AudioDevice::RetCode::Success;
    }

    auto LinuxAudioDevice::Stop() -> RetCode
    {
        return AudioDevice::RetCode::Success;
    }

    auto LinuxAudioDevice::setOutputVolume(float vol) -> RetCode
    {
        vol = std::clamp(vol, minVolume, maxVolume);
        /// Using y=x^4 function as an approximation seems very natural and sufficient
        /// For more info check: https://www.dr-lex.be/info-stuff/volumecontrols.html
        volumeFactor = std::pow(1.0f * (vol / maxVolume), 4);
        return RetCode::Success;
    }

    auto LinuxAudioDevice::setInputGain([[maybe_unused]] float gain) -> RetCode
    {
        return RetCode::Success;
    }

    auto LinuxAudioDevice::getTraits() const -> Traits
    {
        return Traits{};
    }

    auto LinuxAudioDevice::getSupportedFormats() -> std::vector<audio::AudioFormat>
    {
        return supportedFormats;
    }

    auto LinuxAudioDevice::getSourceFormat() -> audio::AudioFormat
    {
        return currentFormat;
    }

    void LinuxAudioDevice::onDataSend()
    {
        audio::Stream::Span dataSpan;
        if (!isSinkConnected()) {
            return;
        }
        Sink::_stream->peek(dataSpan);
        scaleVolume(dataSpan);
        stream->insert(dataSpan);
        Sink::_stream->consume();

        if (stream->bytes() >= requestedBytes) {
            stream->consume();
        }
        else {
            audioProxy.post(requestedBytes);
        }
    }

    void LinuxAudioDevice::onDataReceive()
    {}

    void LinuxAudioDevice::enableInput()
    {}

    void LinuxAudioDevice::enableOutput()
    {
        currentFormat = Sink::_stream->getOutputTraits().format;

        stream = get_context().open_stream(currentFormat, [this](const std::size_t size) {
            requestedBytes = size;
            audioProxy.post(requestedBytes);
        });

        if (stream == nullptr) {
            fprintf(stderr, "Failed to open the audio stream");
            std::abort();
        }
    }

    void LinuxAudioDevice::disableInput()
    {}

    void LinuxAudioDevice::disableOutput()
    {
        get_context().close_stream();
        currentFormat = {};
    }
    void LinuxAudioDevice::scaleVolume(audio::AbstractStream::Span data)
    {
        const auto samplesBeg = reinterpret_cast<std::int16_t *>(data.data);
        const auto samplesEnd = samplesBeg + data.dataSize / 2;
        std::for_each(samplesBeg, samplesEnd, [this](auto &sample) { sample *= volumeFactor; });
    }
} // namespace audio
