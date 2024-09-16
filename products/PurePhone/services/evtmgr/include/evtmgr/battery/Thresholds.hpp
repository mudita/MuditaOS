// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Units.hpp>

namespace constants
{
    static constexpr units::Percent criticalThreshold = 5;
    static constexpr units::Percent shutdownThreshold = 1;

    static constexpr units::Voltage shutdownVoltageThreshold = 3600;
    static constexpr std::uint8_t measurementThreshold       = 5;
} // namespace constants
