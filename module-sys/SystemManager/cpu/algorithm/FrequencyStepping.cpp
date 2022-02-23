// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrequencyStepping.hpp"
#include "SystemManager/CpuGovernor.hpp"

namespace sys::cpu
{

    FrequencyStepping::FrequencyStepping(const bsp::PowerProfile &powerProfile, CpuGovernor &cpuGovernor)
        : powerProfile(powerProfile), cpuGovernor(cpuGovernor)
    {}

    bsp::CpuFrequencyMHz stepDown(bsp::CpuFrequencyMHz freq, const bsp::PowerProfile &profile)
    {
        switch (freq) {
        case bsp::CpuFrequencyMHz::Level_6:
            return bsp::CpuFrequencyMHz::Level_5;
        case bsp::CpuFrequencyMHz::Level_5:
            return bsp::CpuFrequencyMHz::Level_4;
        case bsp::CpuFrequencyMHz::Level_4:
            return bsp::CpuFrequencyMHz::Level_3;
        case bsp::CpuFrequencyMHz::Level_3:
            return bsp::CpuFrequencyMHz::Level_2;
        case bsp::CpuFrequencyMHz::Level_2:
            [[fallthrough]];
        case bsp::CpuFrequencyMHz::Level_1:
            [[fallthrough]];
        case bsp::CpuFrequencyMHz::Level_0:
            return profile.minimalFrequency;
        }
        return freq;
    }

    bsp::CpuFrequencyMHz  FrequencyStepping::calculateImplementation(const AlgorithmData&data)
    {
        auto load           = data.CPUload;
        auto startFrequency = data.curentFrequency;
        const auto min      = cpuGovernor.GetMinimumFrequencyRequested();

        if (load > powerProfile.frequencyShiftUpperThreshold && startFrequency < bsp::CpuFrequencyMHz::Level_6) {
            aboveThresholdCounter++;
            belowThresholdCounter = 0;
        }
        else if (load < powerProfile.frequencyShiftLowerThreshold &&
                 startFrequency > powerProfile.minimalFrequency) {
            belowThresholdCounter++;
            aboveThresholdCounter = 0;
        }
        else {
            reset();
        }

        if (belowThresholdCounter == 0u) {
            isFrequencyLoweringInProgress = false;
        }

        if (min.frequency > startFrequency) {}
        else if (aboveThresholdCounter >= powerProfile.maxAboveThresholdCount) {
            if (startFrequency < bsp::CpuFrequencyMHz::Level_4) {
                reset();
                return bsp::CpuFrequencyMHz::Level_4;
            }
            else {
                reset();
                return bsp::CpuFrequencyMHz::Level_6;
            }
        }
        else {
            if (belowThresholdCounter >= (isFrequencyLoweringInProgress ? powerProfile.maxBelowThresholdInRowCount
                                                                        : powerProfile.maxBelowThresholdCount) &&
                startFrequency > min.frequency) {
                reset();
                return stepDown(startFrequency, powerProfile);
            }
        }

        return startFrequency;
    }

    void FrequencyStepping::resetImplementation()
    {
        aboveThresholdCounter = 0;
        belowThresholdCounter = 0;
    }
}
