/*
 *  @file ProfilePlaybackHeadphones.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_PROFILEPLAYBACKHEADPHONES_HPP
#define PUREPHONE_PROFILEPLAYBACKHEADPHONES_HPP


#include "Profile.hpp"

class ProfilePlaybackHeadphones: public Profile {
public:
    ProfilePlaybackHeadphones(std::function<int32_t()> callback, uint32_t volume, int8_t gain) : Profile(
            "Playback Headphones",Type::PlaybackHeadphones, volume, gain, 0, 0,bsp::AudioDevice::Type::Audiocodec, callback) {}
};

#endif //PUREPHONE_PROFILEPLAYBACKHEADPHONES_HPP
