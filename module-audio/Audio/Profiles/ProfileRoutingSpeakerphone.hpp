// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRoutingSpeakerphone : public Profile
    {
      public:
        ProfileRoutingSpeakerphone(std::function<int32_t()> callback, Volume volume, Gain gain)
            : Profile("Routing Speakerphone",
                      Type::RoutingSpeakerphone,
                      bsp::AudioDevice::Format{
                          .sampleRate_Hz = 16000,
                          .bitWidth      = 16,
                          .flags         = static_cast<uint32_t>(
                                       bsp::AudioDevice::Flags::InputLeft) | // microphone use left audio channel
                                   static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputMono),
                          .outputVolume = static_cast<float>(volume),
                          .inputGain    = static_cast<float>(gain),
                          .inputPath    = bsp::AudioDevice::InputPath::Microphone,
                          .outputPath   = bsp::AudioDevice::OutputPath::Loudspeaker},
                      bsp::AudioDevice::Type::Audiocodec,
                      callback)
        {}
    };

} // namespace audio
