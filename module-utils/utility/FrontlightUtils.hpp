// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>

namespace utils::frontlight
{
#if defined(CONFIG_VERSION_PRO) && (CONFIG_VERSION_PRO == 1)
    inline constexpr auto minimumLightOnPercentOffsetValue = 8.0f;
#else
    inline constexpr auto minimumLightOnPercentOffsetValue = 16.0f;
#endif
    inline constexpr auto minPercent    = 0.0f;
    inline constexpr auto maxPercent    = 100.0f;
    inline constexpr auto minBrightness = 1U;
    inline constexpr auto maxBrightness = 10U;
    inline constexpr float multiplier   = (maxPercent - minimumLightOnPercentOffsetValue) / maxBrightness;

    static inline float fixedValToPercentage(std::uint8_t value)
    {
        const float valueScaled =
            (static_cast<float>(value) - minBrightness) * multiplier + minimumLightOnPercentOffsetValue;
        return std::clamp(valueScaled, minPercent, maxPercent);
    }

    static inline std::uint8_t percentageToFixedVal(float percent)
    {
        const float value = (percent - minimumLightOnPercentOffsetValue) / multiplier;
        return static_cast<std::uint8_t>(std::round(value + minBrightness));
    }
} // namespace utils::frontlight
