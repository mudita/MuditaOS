// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/lpm/PowerProfile.hpp>

namespace bsp
{
    const PowerProfile getPowerProfile()
    {
        PowerProfile purePowerProfile;

        purePowerProfile.frequencyShiftLowerThreshold      = 50;
        purePowerProfile.frequencyShiftUpperThreshold      = 80;
        purePowerProfile.maxBelowThresholdCount            = 5;
        purePowerProfile.maxBelowThresholdInRowCount       = 1;
        purePowerProfile.maxAboveThresholdCount            = 2;
        purePowerProfile.minimalFrequency                  = CpuFrequencyMHz::Level_0;
        purePowerProfile.frequencyIncreaseIntermediateStep = true;

        return purePowerProfile;
    }
} // namespace bsp
