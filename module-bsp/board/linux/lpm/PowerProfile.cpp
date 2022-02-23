// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        linuxPowerProfile.minimalFrequency                  = CpuFrequencyMHz::Level_1;

        return linuxPowerProfile;
    }
} // namespace bsp
