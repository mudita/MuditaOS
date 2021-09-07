// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfilePlaybackLoudspeaker : public Profile
    {
      public:
        ProfilePlaybackLoudspeaker(Volume volume)
            : Profile("Playback Loudspeaker",
                      Type::PlaybackLoudspeaker,
                      audio::codec::Configuration{
                          .sampleRate_Hz = 0,
                          .bitWidth      = 16,
                          .flags         = 0,
                          .outputVolume  = static_cast<float>(volume),
                          .inputGain     = 0,
                          .inputPath     = audio::codec::InputPath::None,
                          .outputPath    = audio::codec::OutputPath::Loudspeaker,
                          .filterCoefficients =
                              {qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterHighPass, 501.8f, 44100, 0.701f, 0),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterLowPass, 14999.5f, 44100, 0.701f, 0),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterHighShelf, 15975.7f, 44100, 0.701f, -10),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterLowShelf, 401.f, 44100, 0.701f, -10),
                               qfilter_CalculateCoeffs(
                                   audio::equalizer::FilterType::FilterParametric, 1496.7f, 44100, 0.701f, -4)}},
                      AudioDevice::Type::Audiocodec)
        {}
    };

} // namespace audio
