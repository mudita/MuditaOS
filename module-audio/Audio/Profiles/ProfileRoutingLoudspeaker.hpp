// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
                      purefs::dir::getCurrentOSPath() / "user/data/equalizer/loudspeaker_routing.json",
                      audio::codec::Configuration{
                          .sampleRate_Hz = sampleRate,
                          .bitWidth      = 16,
                          .flags         = static_cast<uint32_t>(
                                       audio::codec::Flags::InputLeft) | // microphone use left audio channel
                                   static_cast<uint32_t>(audio::codec::Flags::OutputMono),
                          .outputVolume      = 1,
                          .inputGain         = 0,
                          .playbackPathGain  = 0,
                          .playbackPathAtten = 2,
                          .inputPath         = audio::codec::InputPath::Microphone,
                          .outputPath        = audio::codec::OutputPath::Loudspeaker,
                          .filterCoefficients =
                              {qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::HighPass, 501.8f, sampleRate, 0.701f, 0),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::Parametric, 2800.f, sampleRate, 3.f, -4),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::Parametric, 4500.f, sampleRate, 3.f, -6),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::Parametric, 7100.f, sampleRate, 3.f, -3),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::Parametric, 2000.f, sampleRate, 2.f, 6)}},
                      AudioDevice::Type::Audiocodec)
        {
            audioConfiguration.sampleRate_Hz = sampleRate;
            audioConfiguration.outputVolume  = static_cast<float>(volume);
            audioConfiguration.inputGain     = static_cast<float>(gain);
        }
    };

} // namespace audio
