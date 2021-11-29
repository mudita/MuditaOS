// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/lpm/PowerProfile.hpp>

namespace bsp
{
    const PowerProfile getPowerProfile()
    {
        PowerProfile purePowerProfile;

        purePowerProfile.frequencyShiftLowerThreshold      = 40;
        purePowerProfile.frequencyShiftUpperThreshold      = 60;
        purePowerProfile.maxBelowThresholdCount            = 30;
        purePowerProfile.maxBelowThresholdInRowCount       = 10;
        purePowerProfile.maxAboveThresholdCount            = 3;
        purePowerProfile.minimalFrequency                  = CpuFrequencyMHz::Level_1;
        purePowerProfile.frequencyIncreaseIntermediateStep = false;

        return purePowerProfile;
    }
} // namespace bsp
