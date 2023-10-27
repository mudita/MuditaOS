// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
        unsigned int aboveThresholdCounter    = 0;
        unsigned int belowThresholdCounter    = 0;
        bool isFrequencyDownscalingInProgress = true;

      public:
        explicit FrequencyStepping(const bsp::PowerProfile &powerProfile);
        [[nodiscard]] AlgorithmResult calculateImplementation(const AlgorithmData &data) override;
        void resetImplementation() override;
    };
} // namespace sys::cpu
