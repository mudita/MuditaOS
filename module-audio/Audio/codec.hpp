// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

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
        std::uint32_t sampleRate_Hz = 0; /*!< Sample rate of audio data */
        std::uint32_t bitWidth      = 0; /*!< Data length of audio data, usually 8/16/24/32 bits */
        std::uint32_t flags         = 0; /*!< In/Out configuration flags */
        float outputVolume          = 0.0f;
        float inputGain             = 0.0f;
        InputPath inputPath         = InputPath::None;
        OutputPath outputPath       = OutputPath::None;
    };

} // namespace audio::codec
