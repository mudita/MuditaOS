// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Units.hpp>
#include <optional>

constexpr inline auto battery_shutdown_threshold = 5; /// %

std::optional<units::SOC> scale_soc(units::SOC soc);
