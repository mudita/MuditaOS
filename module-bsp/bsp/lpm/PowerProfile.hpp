// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/common.hpp>
#include <cstdint>

namespace bsp
{
    struct PowerProfile
    {
        std::uint32_t frequencyShiftLowerThreshold;
        std::uint32_t frequencyShiftUpperThreshold;
        std::uint32_t maxBelowThresholdCount;
        std::uint32_t maxBelowThresholdInRowCount;
        std::uint32_t maxAboveThresholdCount;
        CpuFrequencyHz minimalFrequency;
        bool frequencyIncreaseIntermediateStep;
    };

    const PowerProfile getPowerProfile();
} // namespace bsp
