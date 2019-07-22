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

    LinuxAudiocodec::LinuxAudiocodec() : Audio(), stream(nullptr) {
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
            return;
        }


        isInitialized = true;
    }

    LinuxAudiocodec::~LinuxAudiocodec() {

        PaError err = paNoError;

        if(stream) {
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


        if(format.flags & static_cast<uint32_t >(Audio::Flags::InputLeft)){
            inChan++;
        }

        if(format.flags & static_cast<uint32_t >(Audio::Flags::InputRight)){
            inChan++;
        }

        if(format.flags & static_cast<uint32_t >(Audio::Flags::OutputMono)){
            outChan++;
        }

        if(format.flags & static_cast<uint32_t >(Audio::Flags::OutPutStereo)){
            outChan = 2;
        }

        /* Open an audio I/O stream. */
        PaError err = Pa_OpenDefaultStream(&stream,
                                   inChan,          /* no input channels */
                                   outChan,          /* stereo output */
                                   paInt16,  /* TODO:M.P only 16bit samples are supported */
                                   format.sampleRate_Hz,
                                   format.dataSize,        /* frames per buffer, i.e. the number
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

        return paNoError;
    }

    int32_t LinuxAudiocodec::Stop() {
        PaError err = Pa_StopStream(stream);
        if (err != paNoError) {
            LOG_ERROR("PortAudio error: %s\n", Pa_GetErrorText(err));
        }

        stream = nullptr;
        format ={};

        return 0;
    }

    int LinuxAudiocodec::portAudioCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                                        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
                                        void *userData)
    {

        //TODO: M.P
        return 0;
    }

    int32_t LinuxAudiocodec::InputGainCtrl(int8_t gain) {
        return 0;
    }

    int32_t LinuxAudiocodec::OutputVolumeCtrl(uint32_t volume) {
        return 0;
    }

    int32_t LinuxAudiocodec::InputPathCtrl([[maybe_unused]] uint32_t inputPath) {
        return 0;
    }

    int32_t LinuxAudiocodec::OutputPathCtrl([[maybe_unused]] uint32_t outputPath) {
        return 0;
    }

    int32_t LinuxAudiocodec::Pause() {
        return 0;
    }

    int32_t LinuxAudiocodec::Resume() {
        return 0;
    }

}
