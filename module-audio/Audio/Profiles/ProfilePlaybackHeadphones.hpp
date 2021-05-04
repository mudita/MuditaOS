// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfilePlaybackHeadphones : public Profile
    {
      public:
        ProfilePlaybackHeadphones(Volume volume)
            : Profile("Playback Headphones",
                      Type::PlaybackHeadphones,
                      AudioDevice::Configuration{.sampleRate_Hz = 0,
                                                 .bitWidth      = 16,
                                                 .flags         = 0,
                                                 .outputVolume  = static_cast<float>(volume),
                                                 .inputGain     = 0,
                                                 .inputPath     = AudioDevice::InputPath::None,
                                                 .outputPath    = AudioDevice::OutputPath::Headphones},
                      AudioDevice::Type::Audiocodec)
        {}
    };

} // namespace audio
