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
    ProfilePlaybackLoudspeaker(std::function<int32_t()> callback, float volume) : Profile(
            "Playback Loudspeaker",Type::PlaybackLoudspeaker, volume, 0.0, 0, 0, bsp::AudioDevice::Type::Audiocodec, callback) {}
};

#endif //PUREPHONE_PROFILEPLAYBACKLOUDSPEAKER_HPP
