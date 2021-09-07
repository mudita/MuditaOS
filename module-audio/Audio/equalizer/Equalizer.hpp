// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <tuple>

namespace audio::equalizer
{
    struct QFilterCoefficients
    {
        float b0;
        float b1;
        float b2;
        float a1;
        float a2;
    };

    using Equalizer = std::array<QFilterCoefficients, 5>;

    enum class FilterType
    {
        FilterBandPass,
        FilterHighPass,
        FilterLowPass,
        FilterFlat,
        FilterNotch,
        FilterLowShelf,
        FilterHighShelf,
        FilterParametric,
        FilterNone
    };

    QFilterCoefficients qfilter_CalculateCoeffs(
        FilterType filter, float frequency, uint32_t samplerate, float Q, float gain);
} // namespace audio::equalizer
