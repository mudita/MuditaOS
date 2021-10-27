// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <algorithm>
#include <optional>

namespace battery_utils
{
    struct BatteryLevelEntry
    {
        using Range = std::pair<std::uint32_t, std::uint32_t>;
        Range realLvl;
        Range displayedLvl;
        std::string_view image;
    };

    struct ScaledBatteryLevel
    {
        std::uint32_t level;
        std::string image;
    };

    template <std::size_t N>
    [[nodiscard]] std::optional<ScaledBatteryLevel> getScaledBatteryLevel(
        const std::array<BatteryLevelEntry, N> &entries, const std::uint32_t val)
    {
        auto result = std::find_if(entries.begin(), entries.end(), [val](const auto &e) {
            return val >= e.realLvl.first && val <= e.realLvl.second;
        });

        if (result != entries.end()) {
            const auto outputStart = result->displayedLvl.first;
            const auto outputEnd   = result->displayedLvl.second;
            const auto inputStart  = result->realLvl.first;
            const auto inputEnd    = result->realLvl.second;
            float slope            = 1.0 * (outputEnd - outputStart) / (inputEnd - inputStart);
            auto output            = outputStart + slope * (val - inputStart);

            return ScaledBatteryLevel{static_cast<std::uint32_t>(std::floor(output)), result->image.data()};
        }

        return {};
    }

} // namespace battery_utils
