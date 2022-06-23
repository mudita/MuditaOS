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
                      .sampleRate_Hz     = 0,
                      .bitWidth          = 16,
                      .flags             = 0,
                      .outputVolume      = 1,
                      .inputGain         = 0,
                      .playbackPathGain  = 0,
                      .playbackPathAtten = 2,
                      .inputPath         = audio::codec::InputPath::None,
                      .outputPath        = audio::codec::OutputPath::Loudspeaker,
                      .filterCoefficients =
                          {qfilter_CalculateCoeffs(audio::equalizer::FilterType::HighPass, 501.8f, 44100, 0.701f, 0),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::Parametric, 2800.f, 44100, 3.f, -4),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::Parametric, 4500.f, 44100, 3.f, -6),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::Parametric, 7100.f, 44100, 3.f, -3),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::Parametric, 2000.f, 44100, 2.f, 6)}},
                  AudioDevice::Type::Audiocodec)
        {
            audioConfiguration.outputVolume = static_cast<float>(volume);
        }
    };

} // namespace audio
