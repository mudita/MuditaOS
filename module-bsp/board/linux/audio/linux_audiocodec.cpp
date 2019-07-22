/*
 *  @file linux_audio.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 22.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "linux_audiocodec.hpp"

#include "log/log.hpp"


namespace bsp {

    LinuxAudiocodec::LinuxAudiocodec(audioCallback_t callback) : Audio(callback), stream(nullptr) {
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
            return;
        }


        isInitialized = true;
    }

    LinuxAudiocodec::~LinuxAudiocodec() {

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

    int32_t LinuxAudiocodec::Start(const bsp::Audio::AudioFormat &format) {

        uint32_t outChan = 0;
        uint32_t inChan = 0;


        if (format.flags & static_cast<uint32_t >(Audio::Flags::InputLeft)) {
            inChan++;
        }

        if (format.flags & static_cast<uint32_t >(Audio::Flags::InputRight)) {
            inChan++;
        }

        if (format.flags & static_cast<uint32_t >(Audio::Flags::OutputMono)) {
            outChan++;
        }

        if (format.flags & static_cast<uint32_t >(Audio::Flags::OutPutStereo)) {
            outChan = 2;
        }

        /* Open an audio I/O stream. */
        PaError err = Pa_OpenDefaultStream(&stream,
                                           inChan,          /* no input channels */
                                           outChan,          /* stereo output */
                                           paInt16,  /* TODO:M.P only 16bit samples are supported */
                                           format.sampleRate_Hz,
                                           paFramesPerBufferUnspecified, /* frames per buffer, i.e. the number
                                                   of sample frames that PortAudio will
                                                   request from the callback. Many apps
                                                   may want to use
                                                   paFramesPerBufferUnspecified, which
                                                   tells PortAudio to pick the best,
                                                   possibly changing, buffer size.*/
                                           portAudioCallback, /* this is your callback function */
                                           this); /*This is a pointer that will be passed to
                                                   your callback*/
        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
            return err;
        }


        err = Pa_StartStream(stream);
        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
            return err;
        }

        currentFormat = format;

        return paNoError;
    }

    int32_t LinuxAudiocodec::Stop() {
        PaError err = Pa_StopStream(stream);
        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
        }

        stream = nullptr;
        currentFormat = {};
        pauseResumeFormat = {};

        return 0;
    }

    int LinuxAudiocodec::portAudioCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                                           const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
                                           void *userData) {

        LinuxAudiocodec* ptr = reinterpret_cast<LinuxAudiocodec*>(userData);

        if(inputBuffer){
            //TODO:M.P volume scaling
        }

        if(outputBuffer){
            //TODO:M.P volume scaling
        }

        return ptr->callback(inputBuffer,outputBuffer,framesPerBuffer);
    }

    int32_t LinuxAudiocodec::InputGainCtrl(int8_t gain) {
        inputGain = gain;
        return 0;
    }

    int32_t LinuxAudiocodec::OutputVolumeCtrl(uint32_t vol) {
        if (vol >= 100) {
            outputVolume = 1.0;
        } else {
            outputVolume = vol / 100;
        }

        return 0;
    }

    int32_t LinuxAudiocodec::InputPathCtrl([[maybe_unused]] uint32_t inputPath) {
        return 0;
    }

    int32_t LinuxAudiocodec::OutputPathCtrl([[maybe_unused]] uint32_t outputPath) {
        return 0;
    }

    int32_t LinuxAudiocodec::Pause() {
        pauseResumeFormat = currentFormat;
        return Stop();
    }

    int32_t LinuxAudiocodec::Resume() {
        return Start(pauseResumeFormat);
    }

    int8_t LinuxAudiocodec::GetInputGain() {
        return inputGain;
    }

    uint32_t LinuxAudiocodec::GetOutputVolume() {
        return outputVolume;
    }

}
