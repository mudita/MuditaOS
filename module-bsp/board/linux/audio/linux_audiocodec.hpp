// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_LINUX_AUDIOCODEC_HPP
#define PUREPHONE_LINUX_AUDIOCODEC_HPP

#include "bsp/audio/bsp_audio.hpp"
#include "portaudio.h"

namespace bsp
{

    class LinuxAudiocodec : public AudioDevice
    {
      public:
        LinuxAudiocodec(AudioDevice::audioCallback_t callback);
        virtual ~LinuxAudiocodec();

        AudioDevice::RetCode Start(const Format &format) override final;
        AudioDevice::RetCode Stop() override final;
        AudioDevice::RetCode OutputVolumeCtrl(float vol) override final;
        AudioDevice::RetCode InputGainCtrl(float gain) override final;
        AudioDevice::RetCode OutputPathCtrl(OutputPath outputPath) override final;
        AudioDevice::RetCode InputPathCtrl(InputPath inputPath) override final;
        bool IsFormatSupported(const Format &format) override final;

        void onDataReceive() final;
        void onDataSend() final;
        void enableInput() final;
        void enableOutput() final;
        void disableInput() final;
        void disableOutput() final;

      private:
        PaStream *stream;

        static int portAudioCallback(const void *inputBuffer,
                                     void *outputBuffer,
                                     unsigned long framesPerBuffer,
                                     const PaStreamCallbackTimeInfo *timeInfo,
                                     PaStreamCallbackFlags statusFlags,
                                     void *userData);

        bool TryOpenStream(const bsp::AudioDevice::Format &format);
    };
} // namespace bsp

#endif // PUREPHONE_LINUX_AUDIOCODEC_HPP
