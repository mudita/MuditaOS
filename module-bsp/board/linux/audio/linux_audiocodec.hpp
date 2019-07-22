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

#include "audio/bsp_audio.hpp"

namespace bsp{


    class LinuxAudiocodec : public Audio{
    public:
        LinuxAudiocodec();
        virtual ~LinuxAudiocodec();

        int32_t Start(const AudioFormat& format) override final;
        int32_t Stop() override final;
        int32_t Pause() override final;
        int32_t Resume() override final;
        int32_t OutputVolumeCtrl(uint32_t volume) override final;
        int32_t InputGainCtrl(int8_t gain) override final;
        int32_t OutputPathCtrl(uint32_t outputPath) override final;
        int32_t InputPathCtrl(uint32_t inputPath) override final;
    };
}




#endif //PUREPHONE_LINUX_AUDIOCODEC_HPP
