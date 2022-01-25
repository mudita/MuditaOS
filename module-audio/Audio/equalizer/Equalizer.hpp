// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <tuple>

namespace audio::equalizer
{
    /*
     * Currently we have 5 band EQ configured in the codec chip, although it is possible
     * to use up to 7 band EQ - it is configurable via internal chip registers.
     */
    constexpr inline auto bands = 5;
    struct QFilterCoefficients
    {
        float b0;
        float b1;
        float b2;
        float a1;
        float a2;
        inline bool operator==(const QFilterCoefficients &rhs) const
        {
            if (b0 != rhs.b0) {
                return false;
            }
            if (b1 != rhs.b1) {
                return false;
            }
            if (b2 != rhs.b2) {
                return false;
            }
            if (a1 != rhs.a1) {
                return false;
            }
            if (a2 != rhs.a2) {
                return false;
            }
            return true;
        }
        inline bool operator!=(const QFilterCoefficients &rhs) const
        {
            if (b0 != rhs.b0) {
                return true;
            }
            if (b1 != rhs.b1) {
                return true;
            }
            if (b2 != rhs.b2) {
                return true;
            }
            if (a1 != rhs.a1) {
                return true;
            }
            if (a2 != rhs.a2) {
                return true;
            }
            return false;
        }
    };

    using Equalizer = std::array<QFilterCoefficients, bands>;

    enum class FilterType
    {
        BandPass,
        HighPass,
        LowPass,
        Flat,
        Notch,
        LowShelf,
        HighShelf,
        Parametric,
        None
    };

    QFilterCoefficients qfilter_CalculateCoeffs(
        FilterType filter, float frequency, uint32_t samplerate, float Q, float gain);
} // namespace audio::equalizer
