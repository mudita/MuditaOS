// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrequencyStepping.hpp"

namespace sys::cpu
{

    FrequencyStepping::FrequencyStepping(const bsp::PowerProfile &powerProfile) : powerProfile(powerProfile)
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
        case bsp::CpuFrequencyMHz::Level_1:
        case bsp::CpuFrequencyMHz::Level_0:
            return profile.minimalFrequency;
        }
        return freq;
    }

    AlgorithmResult FrequencyStepping::calculateImplementation(const AlgorithmData &data)
    {
        const auto load           = data.CPUload;
        const auto startFrequency = data.curentFrequency;
        const auto minFrequency   = data.sentinel.minFrequency;

        if (load > powerProfile.frequencyShiftUpperThreshold && startFrequency < bsp::CpuFrequencyMHz::Level_6) {
            aboveThresholdCounter++;
            belowThresholdCounter = 0;
        }
        else if (load < powerProfile.frequencyShiftLowerThreshold && startFrequency > powerProfile.minimalFrequency) {
            belowThresholdCounter++;
            aboveThresholdCounter = 0;
        }
        else {
            reset();
        }

        if (belowThresholdCounter == 0u) {
            isFrequencyDownscalingInProgress = false;
        }

        if (minFrequency > startFrequency) {}
        else if (aboveThresholdCounter >= powerProfile.maxAboveThresholdCount) {
            if (startFrequency < bsp::CpuFrequencyMHz::Level_4) {
                reset();
                return {algorithm::Change::UpScaled, bsp::CpuFrequencyMHz::Level_4};
            }
            else {
                reset();
                return {algorithm::Change::UpScaled, bsp::CpuFrequencyMHz::Level_6};
            }
        }
        else {
            if (belowThresholdCounter >= (isFrequencyDownscalingInProgress ? powerProfile.maxBelowThresholdInRowCount
                                                                           : powerProfile.maxBelowThresholdCount) &&
                startFrequency > minFrequency) {
                isFrequencyDownscalingInProgress = true;
                reset();
                return {algorithm::Change::Downscaled, stepDown(startFrequency, powerProfile)};
            }
        }

        return {algorithm::Change::NoChange, startFrequency};
    }

    void FrequencyStepping::resetImplementation()
    {
        aboveThresholdCounter = 0;
        belowThresholdCounter = 0;
    }
} // namespace sys::cpu
