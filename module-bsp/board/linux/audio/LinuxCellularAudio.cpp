// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LinuxCellularAudio.hpp"

#include "linux_audiocodec.hpp"

#include <log/log.hpp>

namespace bsp
{

    LinuxCellularAudio::LinuxCellularAudio(audioCallback_t callback) : AudioDevice(callback), stream(nullptr)
    {
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
            return;
        }

        isInitialized = true;
    }

    LinuxCellularAudio::~LinuxCellularAudio()
    {

        PaError err = paNoError;

        if (stream) {
            err = Pa_CloseStream(stream);
            if (err != paNoError) {
                LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
            }
        }

        err = Pa_Terminate();
        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
        }
    }

    AudioDevice::RetCode LinuxCellularAudio::Start(const bsp::AudioDevice::Format &format)
    {

        if (!TryOpenStream(format)) {
            LOG_ERROR("PortAudio error: paInternalError");
            return AudioDevice::RetCode::Failure;
        }

        auto err = Pa_StartStream(stream);
        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
            return AudioDevice::RetCode::Failure;
        }

        currentFormat = format;

        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode LinuxCellularAudio::Stop()
    {

        if (stream) {
            PaError err = Pa_CloseStream(stream);
            if (err != paNoError) {
                LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
            }
        }

        stream        = nullptr;
        currentFormat = {};

        return AudioDevice::RetCode::Success;
    }

    int LinuxCellularAudio::portAudioCallback(const void *inputBuffer,
                                              void *outputBuffer,
                                              unsigned long framesPerBuffer,
                                              const PaStreamCallbackTimeInfo *timeInfo,
                                              PaStreamCallbackFlags statusFlags,
                                              void *userData)
    {

        LinuxCellularAudio *ptr = reinterpret_cast<LinuxCellularAudio *>(userData);

        int32_t framesToFetch = framesPerBuffer;

        if ((ptr->currentFormat.flags & static_cast<uint32_t>(Flags::OutputStereo))) {
            framesToFetch = framesPerBuffer * 2;
        }

        if (ptr->currentFormat.flags & static_cast<uint32_t>(Flags::InputStereo)) {
            framesToFetch = framesPerBuffer * 2;
        }

        if (inputBuffer) {
            int16_t *pBuff              = reinterpret_cast<int16_t *>(const_cast<void *>(inputBuffer));
            constexpr float scaleFactor = .1f;
            std::transform(pBuff, pBuff + framesToFetch, pBuff, [ptr, &scaleFactor](int16_t c) -> int16_t {
                return static_cast<float>(c * ptr->currentFormat.inputGain * scaleFactor);
            });
        }

        auto ret = ptr->callback(inputBuffer, outputBuffer, framesToFetch);
        if (ret == 0) {
            // close stream
            return paAbort;
        }
        else if (ret <= framesToFetch) {

            // Scale output buffer
            if (outputBuffer) {
                int16_t *pBuff              = reinterpret_cast<int16_t *>(outputBuffer);
                constexpr float scaleFactor = .1f;
                std::transform(pBuff, pBuff + framesToFetch, pBuff, [ptr](int16_t c) -> int16_t {
                    return static_cast<float>(c * ptr->currentFormat.outputVolume / 10.f);
                });
            }
            return paContinue;
        }

        return paAbort;
    }

    AudioDevice::RetCode LinuxCellularAudio::InputGainCtrl(float gain)
    {
        currentFormat.inputGain = gain;
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode LinuxCellularAudio::OutputVolumeCtrl(float vol)
    {
        currentFormat.outputVolume = vol;
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode LinuxCellularAudio::InputPathCtrl([[maybe_unused]] InputPath inputPath)
    {
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode LinuxCellularAudio::OutputPathCtrl([[maybe_unused]] OutputPath outputPath)
    {
        return AudioDevice::RetCode::Success;
    }

    bool LinuxCellularAudio::TryOpenStream(const bsp::AudioDevice::Format &format)
    {
        uint32_t outChan = 0;
        uint32_t inChan  = 0;

        if (format.flags & static_cast<uint32_t>(AudioDevice::Flags::InputLeft)) {
            inChan++;
        }

        if (format.flags & static_cast<uint32_t>(AudioDevice::Flags::InputRight)) {
            inChan++;
        }

        if (format.flags & static_cast<uint32_t>(AudioDevice::Flags::OutputMono)) {
            outChan++;
        }

        if (format.flags & static_cast<uint32_t>(AudioDevice::Flags::OutputStereo)) {
            outChan = 2;
        }
        /* Open an audio I/O stream. */
        PaError err = Pa_OpenDefaultStream(&stream,
                                           inChan,  /* no input channels */
                                           outChan, /* stereo output */
                                           paInt16, /* TODO:M.P only 16bit samples are supported */
                                           format.sampleRate_Hz,
                                           paFramesPerBufferUnspecified, /* frames per buffer, i.e. the number
                                                   of sample frames that PortAudio will
                                                   request from the callback. Many apps
                                                   may want to use
                                                   paFramesPerBufferUnspecified, which
                                                   tells PortAudio to pick the best,
                                                   possibly changing, buffer size.*/
                                           portAudioCallback,            /* this is your callback function */
                                           this);                        /*This is a pointer that will be passed to
                                                                          your callback*/

        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
            return false;
        }
        else {
            return true;
        }
    }

    bool LinuxCellularAudio::IsFormatSupported(const bsp::AudioDevice::Format &format)
    {
        auto ret = TryOpenStream(format);
        if (ret) {
            Stop();
            return true;
        }
        else {
            return false;
        }
    }

    void LinuxCellularAudio::onDataReceive()
    {}

    void LinuxCellularAudio::onDataSend()
    {}

    void LinuxCellularAudio::enableInput()
    {}

    void LinuxCellularAudio::enableOutput()
    {}

    void LinuxCellularAudio::disableInput()
    {}

    void LinuxCellularAudio::disableOutput()
    {}

} // namespace bsp
