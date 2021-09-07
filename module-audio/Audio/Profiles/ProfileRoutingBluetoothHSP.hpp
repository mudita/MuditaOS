// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"

namespace audio
{
    class ProfileRoutingBluetoothHSP : public Profile
    {
      public:
        ProfileRoutingBluetoothHSP(Volume volume, Gain gain)
            : Profile("Routing Bluetooth HSP",
                      Type::RoutingBluetoothHSP,
                      audio::codec::Configuration{
                          .sampleRate_Hz = 8000,
                          .bitWidth      = 16,
                          .flags         = static_cast<uint32_t>(
                                       audio::codec::Flags::InputLeft) | // microphone use left audio channel
                                   static_cast<uint32_t>(audio::codec::Flags::OutputMono),
                          .outputVolume = static_cast<float>(volume),
                          .inputGain    = static_cast<float>(gain),
                          .inputPath    = audio::codec::InputPath::None,
                          .outputPath   = audio::codec::OutputPath::None},
                      AudioDevice::Type::BluetoothHSP)
        {}
    };

} // namespace audio
