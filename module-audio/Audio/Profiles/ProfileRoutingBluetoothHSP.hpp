// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
                      bsp::AudioDevice::Format{
                          .sampleRate_Hz = 8000,
                          .bitWidth      = 16,
                          .flags         = static_cast<uint32_t>(
                                       bsp::AudioDevice::Flags::InputLeft) | // microphone use left audio channel
                                   static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputMono),
                          .outputVolume = static_cast<float>(volume),
                          .inputGain    = static_cast<float>(gain),
                          .inputPath    = bsp::AudioDevice::InputPath::BluetoothHSP,
                          .outputPath   = bsp::AudioDevice::OutputPath::BluetoothHSP},
                      bsp::AudioDevice::Type::Bluetooth)
        {}
    };

} // namespace audio
