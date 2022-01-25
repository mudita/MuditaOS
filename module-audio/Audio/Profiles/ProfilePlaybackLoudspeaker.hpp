// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfilePlaybackLoudspeaker : public Profile
    {
      public:
        explicit ProfilePlaybackLoudspeaker(Volume volume)
            : Profile(
                  "Playback Loudspeaker",
                  Type::PlaybackLoudspeaker,
                  purefs::dir::getUserDiskPath() / "data/equalizer/loudspeaker_playback.json",
                  audio::codec::Configuration{
                      .sampleRate_Hz = 0,
                      .bitWidth      = 16,
                      .flags         = 0,
                      .outputVolume  = 0,
                      .inputGain     = 0,
                      .inputPath     = audio::codec::InputPath::None,
                      .outputPath    = audio::codec::OutputPath::Loudspeaker,
                      .filterCoefficients =
                          {qfilter_CalculateCoeffs(audio::equalizer::FilterType::HighPass, 501.8f, 44100, 0.701f, 0),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::LowPass, 14999.5f, 44100, 0.701f, 0),
                           qfilter_CalculateCoeffs(
                               audio::equalizer::FilterType::HighShelf, 15975.7f, 44100, 0.701f, -10),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::LowShelf, 401.f, 44100, 0.701f, -10),
                           qfilter_CalculateCoeffs(
                               audio::equalizer::FilterType::Parametric, 1496.7f, 44100, 0.701f, -4)}},
                  AudioDevice::Type::Audiocodec)
        {
            audioConfiguration.outputVolume = static_cast<float>(volume);
        }
    };

} // namespace audio
