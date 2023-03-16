// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Units.hpp>

namespace constants
{
    static constexpr units::Percent criticalThreshold = 1;
    static constexpr units::Percent shutdownThreshold = 1;

    static constexpr units::Voltage shutdownVoltageThreshold = 3400;
    static constexpr std::uint8_t measurementThreshold       = 0;
} // namespace constants
