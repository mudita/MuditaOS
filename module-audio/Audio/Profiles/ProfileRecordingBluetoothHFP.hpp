// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRecordingBluetoothHFP : public Profile
    {
      public:
        ProfileRecordingBluetoothHFP(Gain gain)
            : Profile(
                  "Recording Bluetooth HFP",
                  Type::RecordingHeadphones,
                  audio::codec::Configuration{.sampleRate_Hz = 8000,
                                              .bitWidth      = 16,
                                              .flags         = static_cast<uint32_t>(
                                                  audio::codec::Flags::InputLeft), // microphone use left audio channel
                                              .outputVolume = 0,
                                              .inputGain    = static_cast<float>(gain),
                                              .inputPath    = audio::codec::InputPath::None,
                                              .outputPath   = audio::codec::OutputPath::None},
                  AudioDevice::Type::BluetoothHFP)
        {}
    };

} // namespace audio
