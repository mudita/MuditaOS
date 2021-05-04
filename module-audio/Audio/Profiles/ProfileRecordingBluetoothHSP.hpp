// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRecordingBluetoothHSP : public Profile
    {
      public:
        ProfileRecordingBluetoothHSP(Gain gain)
            : Profile("Recording Bluetooth HSP",
                      Type::RecordingHeadphones,
                      AudioDevice::Configuration{
                          .sampleRate_Hz = 8000,
                          .bitWidth      = 16,
                          .flags =
                              static_cast<uint32_t>(AudioDevice::Flags::InputLeft), // microphone use left audio channel
                          .outputVolume = 0,
                          .inputGain    = static_cast<float>(gain),
                          .inputPath    = AudioDevice::InputPath::None,
                          .outputPath   = AudioDevice::OutputPath::None},
                      AudioDevice::Type::Bluetooth)
        {}
    };

} // namespace audio
