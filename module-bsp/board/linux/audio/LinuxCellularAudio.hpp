/*
 *  @file LinuxCellularAudio.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 02.08.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_LINUXCELLULARAUDIO_HPP
#define PUREPHONE_LINUXCELLULARAUDIO_HPP

#include "bsp/audio/bsp_audio.hpp"
#include "portaudio.h"

namespace bsp
{

    class LinuxCellularAudio : public AudioDevice
    {
      public:
        LinuxCellularAudio(AudioDevice::audioCallback_t callback);

        virtual ~LinuxCellularAudio();

        int32_t Start(const Format &format) override final;

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

#endif // PUREPHONE_LINUXCELLULARAUDIO_HPP
