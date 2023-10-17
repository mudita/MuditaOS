// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cmath>
#include <cstdint>

namespace frontlight_utils
{
    namespace
    {
        static constexpr auto minPercent                       = 0.0f;
        static constexpr auto maxPercent                       = 100.0f;
        static constexpr auto minimumLightOnPercentOffsetValue = 16.0f;
        static constexpr auto minBrightness                    = 1U;
        static constexpr auto maxBrightness                    = 10U;
        static constexpr float multiplier = (maxPercent - minimumLightOnPercentOffsetValue) / maxBrightness;
    } // namespace

    /// 1-10 range
    using Brightness = std::uint8_t;

    static inline float fixedValToPercentage(Brightness value)
    {
        float scaled = minimumLightOnPercentOffsetValue + (value - minBrightness) * multiplier;
        return std::min(maxPercent, std::max(minPercent, scaled));
    }

    static inline Brightness percentageToFixedVal(float percent)
    {
        auto value = (percent - minimumLightOnPercentOffsetValue) / multiplier;
        return std::round(value + minBrightness);
    }

} // namespace frontlight_utils
