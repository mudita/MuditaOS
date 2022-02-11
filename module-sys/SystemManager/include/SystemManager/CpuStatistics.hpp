// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SystemManager/PowerManager.hpp"
#include "SystemManager/CpuPrinter.hpp"
#include <cstdint>

extern "C"
{
#include "prof.h"
}

namespace sys
{

    class CpuStatistics
    {

      public:
        CpuStatistics();
        ~CpuStatistics();
        /// stores system usage, should be called before any CPU frequency change
        /// this way we know what services were in use and for how long before it happened
        void StoreSysUsage();
        [[nodiscard]] uint32_t GetPercentageCpuLoad() const noexcept;
        void UpdatePercentageCpuLoad();
        void TrackChange(const cpu::UpdateResult &ret);

      private:
        /// used to print stored data in CpuStatistics on change
        std::unique_ptr<cpu::stats::Printer> printer;
        uint32_t ComputeIncrease(uint32_t currentCount, uint32_t lastCount) const;

        uint32_t lastIdleTickCount{0};
        uint32_t lastTotalTickCount{0};
        uint32_t cpuLoad{0};

#if PROF_ON
        size_t data_size{0};
        struct task_prof_data *data = nullptr;
#endif
    };

} // namespace sys
