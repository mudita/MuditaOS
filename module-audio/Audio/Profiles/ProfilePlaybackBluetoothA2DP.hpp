// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfilePlaybackBluetoothA2DP : public Profile
    {
      public:
        ProfilePlaybackBluetoothA2DP(Volume volume)
            : Profile("Playback Bluetooth A2DP",
                      Type::PlaybackBluetoothA2DP,
                      AudioDevice::Configuration{.sampleRate_Hz = 44100,
                                                 .bitWidth      = 16,
                                                 .flags         = 0,
                                                 .outputVolume  = static_cast<float>(volume),
                                                 .inputGain     = 0,
                                                 .inputPath     = AudioDevice::InputPath::None,
                                                 .outputPath    = AudioDevice::OutputPath::None},
                      AudioDevice::Type::Bluetooth)
        {}
    };

} // namespace audio
