// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfilePlaybackLoudspeaker : public Profile
    {
      public:
        ProfilePlaybackLoudspeaker(Volume volume)
            : Profile("Playback Loudspeaker",
                      Type::PlaybackLoudspeaker,
                      AudioDevice::Format{.sampleRate_Hz = 0,
                                          .bitWidth      = 16,
                                          .flags         = 0,
                                          .outputVolume  = static_cast<float>(volume),
                                          .inputGain     = 0,
                                          .inputPath     = AudioDevice::InputPath::None,
                                          .outputPath    = AudioDevice::OutputPath::Loudspeaker},
                      AudioDevice::Type::Audiocodec)
        {}
    };

} // namespace audio
