/*
 *  @file linux_audio.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 22.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

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
        int32_t Stop() override final;
        int32_t OutputVolumeCtrl(float vol) override final;
        int32_t InputGainCtrl(float gain) override final;
        int32_t OutputPathCtrl(OutputPath outputPath) override final;
        int32_t InputPathCtrl(InputPath inputPath) override final;
        bool IsFormatSupported(const Format &format) override final;

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
