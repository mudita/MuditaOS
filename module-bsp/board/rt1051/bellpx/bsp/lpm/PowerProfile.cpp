// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/lpm/PowerProfile.hpp>

namespace bsp
{
    const PowerProfile getPowerProfile()
    {
        PowerProfile bellPowerProfile;

        bellPowerProfile.frequencyShiftLowerThreshold      = 50;
        bellPowerProfile.frequencyShiftUpperThreshold      = 80;
        bellPowerProfile.maxBelowThresholdCount            = 5;
        bellPowerProfile.maxBelowThresholdInRowCount       = 1;
        bellPowerProfile.maxAboveThresholdCount            = 2;
        bellPowerProfile.minimalFrequency                  = CpuFrequencyMHz::Level_0;
        bellPowerProfile.frequencyIncreaseIntermediateStep = true;

        return bellPowerProfile;
    }
} // namespace bsp
