/*
 *  @file ProfilePlaybackLoudspeaker.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_PROFILEPLAYBACKLOUDSPEAKER_HPP
#define PUREPHONE_PROFILEPLAYBACKLOUDSPEAKER_HPP

#include "Profile.hpp"

class ProfilePlaybackLoudspeaker : public Profile {
public:
    ProfilePlaybackLoudspeaker(std::function<int32_t()> callback, uint32_t volume, int8_t gain) : Profile(
            "Playback Loudspeaker",Type::PlaybackLoudspeaker, volume, gain, 0, 0, bsp::AudioDevice::Type::Audiocodec, callback) {}
};

#endif //PUREPHONE_PROFILEPLAYBACKLOUDSPEAKER_HPP
