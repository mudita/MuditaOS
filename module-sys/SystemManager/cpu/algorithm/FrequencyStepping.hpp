// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Algorithm.hpp"
#include "lpm/PowerProfile.hpp"

namespace sys
{
    class CpuGovernor;
}

namespace sys::cpu
{
    class FrequencyStepping : public Algorithm
    {
        const bsp::PowerProfile &powerProfile;
        CpuGovernor &cpuGovernor;
        unsigned int aboveThresholdCounter = 0;
        unsigned int belowThresholdCounter = 0;
        bool isFrequencyLoweringInProgress = true;

      public:
        FrequencyStepping(const bsp::PowerProfile &powerProfile, CpuGovernor &cpuGovernor);
        [[nodiscard]] AlgorithmResult calculateImplementation(const AlgorithmData &data) override;
        void resetImplementation() override;
    };
} // namespace sys::cpu
