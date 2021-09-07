// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfilePlaybackHeadphones : public Profile
    {
      public:
        ProfilePlaybackHeadphones(Volume volume)
            : Profile(
                  "Playback Headphones",
                  Type::PlaybackHeadphones,
                  audio::codec::Configuration{
                      .sampleRate_Hz = 0,
                      .bitWidth      = 16,
                      .flags         = 0,
                      .outputVolume  = static_cast<float>(volume),
                      .inputGain     = 0,
                      .inputPath     = audio::codec::InputPath::None,
                      .outputPath    = audio::codec::OutputPath::Headphones,
                      .filterCoefficients =
                          {qfilter_CalculateCoeffs(
                               audio::equalizer::FilterType::FilterHighPass, 100.2f, 44100, 0.701f, 0),
                           qfilter_CalculateCoeffs(
                               audio::equalizer::FilterType::FilterLowPass, 17996.2f, 44100, 0.701f, 0),
                           qfilter_CalculateCoeffs(
                               audio::equalizer::FilterType::FilterHighShelf, 13984.7f, 44100, 0.701f, -10),
                           qfilter_CalculateCoeffs(
                               audio::equalizer::FilterType::FilterLowShelf, 200.4f, 44100, 0.701f, -10),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::FilterNone, 0, 44100, 0.701f, -4)}},
                  AudioDevice::Type::Audiocodec)
        {}
    };

} // namespace audio
