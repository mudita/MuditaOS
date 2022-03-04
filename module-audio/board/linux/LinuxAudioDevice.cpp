// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LinuxAudioDevice.hpp"
#include <Audio/Stream.hpp>
#include <log/log.hpp>
#include <cmath>

namespace audio
{
    namespace
    {
        class PortAudio
        {
          public:
            PortAudio();
            PortAudio(const PortAudio &) = delete;
            PortAudio(PortAudio &&)      = delete;
            PortAudio &operator=(const PortAudio &) = delete;
            PortAudio &operator=(PortAudio &&) = delete;
            ~PortAudio() noexcept;
        };

        PortAudio::PortAudio()
        {
            if (const auto errorCode = Pa_Initialize(); errorCode == paNoError) {
                LOG_INFO("Portaudio initialized successfully");
            }
            else {
                LOG_ERROR("Error (code %d) initiializing Portaudio: %s", errorCode, Pa_GetErrorText(errorCode));
            }
        }

        PortAudio::~PortAudio() noexcept
        {
            if (const auto errorCode = Pa_Terminate(); errorCode == paNoError) {
                LOG_INFO("Portaudio terminated successfully");
            }
            else {
                LOG_ERROR("Error (code %d) while terminating Portaudio: %s", errorCode, Pa_GetErrorText(errorCode));
            }
        }
    } // namespace

    LinuxAudioDevice::LinuxAudioDevice(const float initialVolume)
        : supportedFormats(
              audio::AudioFormat::makeMatrix(supportedSampleRates, supportedBitWidths, supportedChannelModes))
    {
        setOutputVolume(initialVolume);

        static PortAudio portAudio;
    }

    LinuxAudioDevice::~LinuxAudioDevice()
    {
        if (stream != nullptr) {
            closeStream();
        }
    }

    void LinuxAudioDevice::closeStream()
    {
        if (const auto errorCode = Pa_AbortStream(stream); errorCode != paNoError) {
            LOG_ERROR("Error (code %d) while stopping Portaudio stream: %s", errorCode, Pa_GetErrorText(errorCode));
        }
        if (const auto errorCode = Pa_CloseStream(stream); errorCode != paNoError) {
            LOG_ERROR("Error (code %d) while closing Portaudio stream: %s", errorCode, Pa_GetErrorText(errorCode));
        }
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
        Sink::_stream->peek(dataSpan);
        auto streamData = reinterpret_cast<std::int16_t *>(dataSpan.data);
        cache.insert(cache.end(), &streamData[0], &streamData[dataSpan.dataSize / sizeof(std::int16_t)]);
        Sink::_stream->consume();
    }

    void LinuxAudioDevice::onDataReceive()
    {}

    void LinuxAudioDevice::enableInput()
    {}

    void LinuxAudioDevice::enableOutput()
    {
        LOG_INFO("Enabling audio output...");
        if (!isSinkConnected()) {
            LOG_ERROR("Output stream is not connected!");
            return;
        }

        currentFormat                = Sink::_stream->getOutputTraits().format;
        const auto numOutputChannels = currentFormat.getChannels();
        auto callback                = [](const void *input,
                           void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo *timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData) -> int {
            LinuxAudioDevice *dev = static_cast<LinuxAudioDevice *>(userData);
            return dev->streamCallback(input, output, frameCount, timeInfo, statusFlags);
        };
        auto errorCode = Pa_OpenDefaultStream(&stream,
                                              0,
                                              numOutputChannels,
                                              paInt16,
                                              currentFormat.getSampleRate(),
                                              paFramesPerBufferUnspecified,
                                              callback,
                                              this);
        if (errorCode != paNoError) {
            LOG_ERROR("Error (code %d) while creating portaudio stream: %s", errorCode, Pa_GetErrorText(errorCode));
            return;
        }
        if (errorCode = Pa_StartStream(stream); errorCode != paNoError) {
            LOG_ERROR("Error (code %d) while starting portaudio stream: %s", errorCode, Pa_GetErrorText(errorCode));
            return;
        }
    }

    void LinuxAudioDevice::disableInput()
    {}

    void LinuxAudioDevice::disableOutput()
    {
        LOG_INFO("Disabling audio output...");
        if (!isSinkConnected()) {
            LOG_ERROR("Error while stopping Linux Audio Device! Null stream.");
            return;
        }

        closeStream();
        stream        = nullptr;
        currentFormat = {};
    }

    int LinuxAudioDevice::streamCallback([[maybe_unused]] const void *input,
                                         void *output,
                                         unsigned long frameCount,
                                         [[maybe_unused]] const PaStreamCallbackTimeInfo *timeInfo,
                                         [[maybe_unused]] PaStreamCallbackFlags statusFlags)
    {
        if (!isSinkConnected()) {
            return paAbort;
        }

        const auto expectedBufferSize = frameCount * currentFormat.getChannels();
        if (!isCacheReady(expectedBufferSize)) {
            onDataSend();
        }

        const auto dataReadySize = std::min(expectedBufferSize, cache.size());
        cacheToOutputBuffer(static_cast<std::int16_t *>(output), dataReadySize);

        return paContinue;
    }

    bool LinuxAudioDevice::isCacheReady(std::size_t expectedSize) const noexcept
    {
        return cache.size() >= expectedSize;
    }

    void LinuxAudioDevice::cacheToOutputBuffer(std::int16_t *buffer, std::size_t size)
    {
        for (size_t i = 0; i < size; ++i) {
            const auto adjustedValue = static_cast<float>(cache[i]) * volumeFactor;
            *(buffer)                = static_cast<std::int16_t>(adjustedValue);
            buffer++;
        }
        cache.erase(cache.begin(), cache.begin() + size);
    }
} // namespace audio
