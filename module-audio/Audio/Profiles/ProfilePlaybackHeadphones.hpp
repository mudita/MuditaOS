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

namespace audio
{

    class ProfilePlaybackHeadphones : public Profile
    {
      public:
        ProfilePlaybackHeadphones(std::function<int32_t()> callback, float volume)
            : Profile("Playback Headphones",
                      Type::PlaybackHeadphones,
                      bsp::AudioDevice::Format{.sampleRate_Hz = 0,
                                               .bitWidth      = 16,
                                               .flags         = 0,
                                               .outputVolume  = volume,
                                               .inputGain     = 0,
                                               .inputPath     = bsp::AudioDevice::InputPath::None,
                                               .outputPath    = bsp::AudioDevice::OutputPath::Headphones},
                      bsp::AudioDevice::Type::Audiocodec,
                      callback)
        {}
    };

} // namespace audio

#endif // PUREPHONE_PROFILEPLAYBACKHEADPHONES_HPP
