// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace sys
{

    class CpuStatistics
    {

      public:
        void Update();
        [[nodiscard]] uint32_t GetPercentageCpuLoad() const noexcept;

      private:
        uint32_t ComputeIncrease(uint32_t currentCount, uint32_t lastCount) const;

        uint32_t lastIdleTickCount{0};
        uint32_t lastTotalTickCount{0};
        uint32_t cpuLoad{0};
    };

} // namespace sys
