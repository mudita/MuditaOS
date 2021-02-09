// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRecordingHeadphones : public Profile
    {
      public:
        ProfileRecordingHeadphones(Gain gain)
            : Profile("Recording Headset",
                      Type::RecordingHeadphones,
                      AudioDevice::Format{.sampleRate_Hz = 44100,
                                          .bitWidth      = 16,
                                          .flags         = static_cast<uint32_t>(
                                              AudioDevice::Flags::InputLeft), // microphone use left audio channel
                                          .outputVolume = 0,
                                          .inputGain    = static_cast<float>(gain),
                                          .inputPath    = AudioDevice::InputPath::Headphones,
                                          .outputPath   = AudioDevice::OutputPath::None},
                      AudioDevice::Type::Audiocodec)
        {}
    };

} // namespace audio
