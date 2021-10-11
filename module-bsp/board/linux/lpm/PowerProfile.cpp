// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/lpm/PowerProfile.hpp>

namespace bsp
{
    const PowerProfile getPowerProfile()
    {
        PowerProfile linuxPowerProfile;

        linuxPowerProfile.frequencyShiftLowerThreshold      = 40;
        linuxPowerProfile.frequencyShiftUpperThreshold      = 60;
        linuxPowerProfile.maxBelowThresholdCount            = 30;
        linuxPowerProfile.maxBelowThresholdInRowCount       = 10;
        linuxPowerProfile.maxAboveThresholdCount            = 3;
        linuxPowerProfile.minimalFrequency                  = CpuFrequencyHz::Level_1;
        linuxPowerProfile.frequencyIncreaseIntermediateStep = false;

        return linuxPowerProfile;
    }
} // namespace bsp
