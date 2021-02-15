// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRecordingOnBoardMic : public Profile
    {
      public:
        ProfileRecordingOnBoardMic(Gain gain)
            : Profile(
                  "Recording On Board Microphone",
                  Type::RecordingBuiltInMic,
                  bsp::AudioDevice::Format{.sampleRate_Hz = 44100,
                                           .bitWidth      = 16,
                                           .flags         = static_cast<uint32_t>(
                                               bsp::AudioDevice::Flags::InputLeft), // microphone use left audio channel
                                           .outputVolume = 0,
                                           .inputGain    = static_cast<float>(gain),
                                           .inputPath    = bsp::AudioDevice::InputPath::Microphone,
                                           .outputPath   = bsp::AudioDevice::OutputPath::None},
                  bsp::AudioDevice::Type::Audiocodec)
        {}
    };

} // namespace audio
