// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRoutingHeadphones : public Profile
    {
      public:
        ProfileRoutingHeadphones(Volume volume, Gain gain)
            : Profile("Routing Headset",
                      Type::RoutingHeadphones,
                      audio::codec::Configuration{
                          .sampleRate_Hz = 16000,
                          .bitWidth      = 16,
                          .flags         = static_cast<uint32_t>(
                                       audio::codec::Flags::InputLeft) | // microphone use left audio channel
                                   static_cast<uint32_t>(audio::codec::Flags::OutputMono),
                          .outputVolume = static_cast<float>(volume),
                          .inputGain    = static_cast<float>(gain),
                          .inputPath    = audio::codec::InputPath::Headphones,
                          .outputPath   = audio::codec::OutputPath::Headphones,
                          .filterCoefficients =
                              {qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterHighPass, 997.f, 44100, 0.701f, 0),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterLowPass, 4993.7f, 44100, 0.701f, 0),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterNone, 15975.7f, 44100, 0.701f, -10),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterNone, 200.4f, 44100, 0.701f, -10),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterNone, 1496.7f, 44100, 0.701f, -4)}},
                      AudioDevice::Type::Audiocodec)
        {}
    };

} // namespace audio
