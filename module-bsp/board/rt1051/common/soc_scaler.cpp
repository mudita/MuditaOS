// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "soc_scaler.hpp"

#include <ValueScaler.hpp>

namespace
{
    /// Scale SOC <5% to 0%.
    // clang-format off
    constexpr auto entries = std::array<utils::Entry<std::uint8_t>, 3>{{
            {{0, battery_shutdown_threshold-1}, {0, 0}},
            {{battery_shutdown_threshold, 99}, {1, 99}},
            {{100, 100}, {100, 100}}
    }};
    // clang-format on
} // namespace

std::optional<units::SOC> scale_soc(units::SOC soc)
{
    return utils::find_and_scale_value(entries, soc);
}
