// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/lpm/PowerProfile.hpp>

namespace bsp
{
    const PowerProfile getPowerProfile()
    {
        PowerProfile bellPowerProfile;

        bellPowerProfile.frequencyShiftLowerThreshold      = 50;
        bellPowerProfile.frequencyShiftUpperThreshold      = 90;
        bellPowerProfile.maxBelowThresholdCount            = 10;
        bellPowerProfile.maxBelowThresholdInRowCount       = 5;
        bellPowerProfile.maxAboveThresholdCount            = 3;
        bellPowerProfile.minimalFrequency                  = CpuFrequencyHz::Level_0;
        bellPowerProfile.frequencyIncreaseIntermediateStep = true;

        return bellPowerProfile;
    }
} // namespace bsp
