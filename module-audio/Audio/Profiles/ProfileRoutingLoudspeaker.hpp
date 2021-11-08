// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRoutingLoudspeaker : public Profile
    {
        static constexpr uint32_t sampleRate = 16000;

      public:
        ProfileRoutingLoudspeaker(Volume volume, Gain gain)
            : Profile("Routing Speakerphone",
                      Type::RoutingLoudspeaker,
                      audio::codec::Configuration{
                          .sampleRate_Hz = sampleRate,
                          .bitWidth      = 16,
                          .flags         = static_cast<uint32_t>(
                                       audio::codec::Flags::InputLeft) | // microphone use left audio channel
                                   static_cast<uint32_t>(audio::codec::Flags::OutputMono),
                          .outputVolume = static_cast<float>(volume),
                          .inputGain    = static_cast<float>(gain),
                          .inputPath    = audio::codec::InputPath::Microphone,
                          .outputPath   = audio::codec::OutputPath::Loudspeaker,
                          .filterCoefficients =
                              {qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterHighPass, 307.3f, sampleRate, 0.701f, 0),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterLowPass, 5080.1f, sampleRate, 0.847f, 0),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterNone, 15975.7f, sampleRate, 0.701f, -10),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterNone, 200.4f, sampleRate, 0.701f, -10),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterNone, 1496.7f, sampleRate, 0.701f, -4)}},
                      AudioDevice::Type::Audiocodec)
        {}
    };

} // namespace audio
