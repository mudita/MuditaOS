// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Units.hpp>

namespace constants
{
    inline constexpr units::Percent criticalThreshold = 1;
    inline constexpr units::Percent shutdownThreshold = 1;

    inline constexpr units::Voltage shutdownVoltageThreshold = 3400;
    inline constexpr std::uint8_t measurementThreshold       = 0;
} // namespace constants
