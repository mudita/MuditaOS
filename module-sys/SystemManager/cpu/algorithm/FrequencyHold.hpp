// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Algorithm.hpp"
#include "lpm/PowerProfile.hpp"

namespace sys::cpu
{
    class FrequencyHold : public Algorithm {
            bsp::CpuFrequencyMHz toHold;
            const bsp::PowerProfile &profile;
            [[nodiscard]] bsp::CpuFrequencyMHz calculateImplementation(const AlgorithmData& data) override;
        public:
          explicit FrequencyHold(bsp::CpuFrequencyMHz toHold, const bsp::PowerProfile &profile);
    };
}
