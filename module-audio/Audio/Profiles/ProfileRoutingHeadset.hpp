/*
 *  @file ProfileRoutingHeadset.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 02.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_PROFILEROUTINGHEADSET_HPP
#define PUREPHONE_PROFILEROUTINGHEADSET_HPP

#include "Profile.hpp"

namespace audio {

    class ProfileRoutingHeadset : public Profile {
    public:
        ProfileRoutingHeadset(std::function<int32_t()> callback,float volume, float gain) : Profile(
                "Routing Headset", Type::RoutingHeadset,
                bsp::AudioDevice::Format{.sampleRate_Hz=16000,
                        .bitWidth=16,
                        .flags=static_cast<uint32_t >(bsp::AudioDevice::Flags::InputLeft) | // microphone use left audio channel
                               static_cast<uint32_t >(bsp::AudioDevice::Flags::OutputMono),
                        .outputVolume=volume,
                        .inputGain=gain,
                        .inputPath=bsp::AudioDevice::InputPath::Headphones,
                        .outputPath=bsp::AudioDevice::OutputPath::Headphones
                },
                bsp::AudioDevice::Type::Audiocodec, callback) {}
    };

}

#endif //PUREPHONE_PROFILEROUTINGHEADSET_HPP
