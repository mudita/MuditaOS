// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <algorithm>
#include <optional>
#include <string>

#include <Units.hpp>

namespace battery_utils
{
    struct BatteryLevelEntry
    {
        using Range = std::pair<units::SOC, units::SOC>;
        const Range range;
        std::string_view image;
    };

    template <std::size_t N>
    [[nodiscard]] std::optional<std::string_view> getBatteryLevelImage(const std::array<BatteryLevelEntry, N> &entries,
                                                                       const units::SOC soc)
    {
        auto result = std::find_if(entries.begin(), entries.end(), [soc](const auto &e) {
            return soc >= e.range.first && soc <= e.range.second;
        });

        if (result != entries.end()) {
            return result->image;
        }

        return {};
    }

} // namespace battery_utils
