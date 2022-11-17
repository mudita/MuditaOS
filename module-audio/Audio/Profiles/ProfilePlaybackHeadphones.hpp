// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfilePlaybackHeadphones : public Profile
    {
      public:
        explicit ProfilePlaybackHeadphones(Volume volume)
            : Profile(
                  "Playback Headphones",
                  Type::PlaybackHeadphones,
                  purefs::dir::getSystemDiskPath() / "data/equalizer/headphones_playback.json",
                  audio::codec::Configuration{
                      .sampleRate_Hz = 0,
                      .bitWidth      = 16,
                      .flags         = 0,
                      .outputVolume  = 0,
                      .inputGain     = 0,
                      .inputPath     = audio::codec::InputPath::None,
                      .outputPath    = audio::codec::OutputPath::Headphones,
                      .filterCoefficients =
                          {qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 100.2f, 44100, 0.701f, 0),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 17996.2f, 44100, 0.701f, 0),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 13984.7f, 44100, 0.701f, -10),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 200.4f, 44100, 0.701f, -10),
                           qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 0, 44100, 0.701f, -4)}},
                  AudioDevice::Type::Audiocodec)
        {
            audioConfiguration.outputVolume = static_cast<float>(volume);
        }
    };

} // namespace audio
