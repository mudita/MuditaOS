// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <array>
#include <module-audio/Audio/equalizer/Equalizer.hpp>

namespace audio::codec
{
    enum class Flags
    {
        OutputMono   = 1 << 0,
        OutputStereo = 1 << 1,
        InputLeft    = 1 << 2,
        InputRight   = 1 << 3,
        InputStereo  = 1 << 4
    };

    enum class InputPath
    {
        Headphones,
        Microphone,
        None
    };

    enum class OutputPath
    {
        Headphones,
        Earspeaker,
        Loudspeaker,
        None
    };

    struct Configuration
    {
        std::uint32_t sampleRate_Hz                    = 0; /*!< Sample rate of audio data */
        std::uint32_t bitWidth                         = 0; /*!< Data length of audio data, usually 8/16/24/32 bits */
        std::uint32_t flags                            = 0; /*!< In/Out configuration flags */
        float outputVolume                             = 0.0f;
        float inputGain                                = 0.0f;
        std::uint8_t playbackPathGain                  = 0;
        std::uint8_t playbackPathAtten                 = 5;
        InputPath inputPath                            = InputPath::None;
        OutputPath outputPath                          = OutputPath::None;
        audio::equalizer::Equalizer filterCoefficients = {
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 100.2f, 44100, 0.701f, 0),
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 17996.2f, 44100, 0.701f, 0),
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 13984.7f, 44100, 0.701f, -10),
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 200.4f, 44100, 0.701f, -10),
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 0, 44100, 0.701f, -4)};
    };

} // namespace audio::codec
