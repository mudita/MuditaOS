// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LinuxAudioDevice.hpp"
#include <Audio/Stream.hpp>
#include <log/log.hpp>
#include <cmath>

namespace audio
{
    LinuxAudioDevice::LinuxAudioDevice(const float initialVolume)
        : supportedFormats(
              audio::AudioFormat::makeMatrix(supportedSampleRates, supportedBitWidths, supportedChannelModes)),
          audioProxy("audioProxy", [this](const auto &data) {
              requestedBytes = data;
              onDataSend();
          })
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
        pulseAudioWrapper->insert(dataSpan);
        Sink::_stream->consume();

        if (pulseAudioWrapper->bytes() >= requestedBytes) {
            pulseAudioWrapper->consume();
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
        if (!isSinkConnected()) {
            LOG_ERROR("Output stream is not connected!");
            return;
        }

        currentFormat = Sink::_stream->getOutputTraits().format;

        pulseAudioWrapper = std::make_unique<PulseAudioWrapper>(
            [this](const std::size_t size) { audioProxy.post(size); }, currentFormat);
    }

    void LinuxAudioDevice::disableInput()
    {}

    void LinuxAudioDevice::disableOutput()
    {
        if (!isSinkConnected()) {
            LOG_ERROR("Error while stopping Linux Audio Device! Null stream.");
            return;
        }
        close         = true;
        currentFormat = {};
    }
    void LinuxAudioDevice::scaleVolume(audio::AbstractStream::Span data)
    {
        const auto samplesBeg = reinterpret_cast<std::int16_t *>(data.data);
        const auto samplesEnd = samplesBeg + data.dataSize / 2;
        std::for_each(samplesBeg, samplesEnd, [this](auto &sample) { sample *= volumeFactor; });
    }
} // namespace audio
