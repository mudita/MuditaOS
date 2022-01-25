// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRoutingEarspeaker : public Profile
    {
      public:
        ProfileRoutingEarspeaker(Volume volume, Gain gain)
            : Profile(
                  "Routing Earspeaker",
                  Type::RoutingEarspeaker,
                  purefs::dir::getUserDiskPath() / "data/equalizer/earspeaker_routing.json",
                  audio::codec::Configuration{
                      .sampleRate_Hz = 16000,
                      .bitWidth      = 16,
                      .flags =
                          static_cast<uint32_t>(audio::codec::Flags::InputLeft) | // microphone use left audio channel
                          static_cast<uint32_t>(audio::codec::Flags::OutputMono),
                      .outputVolume = 0,
                      .inputGain    = 0,
                      .inputPath    = audio::codec::InputPath::Microphone,
                      .outputPath   = audio::codec::OutputPath::Earspeaker,
                      .filterCoefficients =
                          {qfilter_CalculateCoeffs(audio::equalizer::FilterType::HighPass, 700.f, 44100, 0.701f, 10),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::LowPass, 4993.7f, 44100, 0.701f, -5),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::LowPass, 6000.7f, 44100, 0.701f, 10),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::HighPass, 100.4f, 44100, 0.701f, 10),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::Notch, 1500.7f, 44100, 0.701f, -3)}},
                  AudioDevice::Type::Audiocodec)
        {
            audioConfiguration.outputVolume = static_cast<float>(volume);
            audioConfiguration.inputGain    = static_cast<float>(gain);
        }
    };

} // namespace audio
