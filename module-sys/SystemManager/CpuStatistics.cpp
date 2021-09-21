// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/CpuStatistics.hpp>
#include <log/log.hpp>
#include <FreeRTOS.h>
#include <task.h>
#include <limits>

namespace sys
{
    void CpuStatistics::Update()
    {
        uint32_t idleTickCount  = xTaskGetIdleRunTimeCounter();
        uint32_t totalTickCount = ulHighFrequencyTimerTicks();

        uint32_t idleTickIncrease  = ComputeIncrease(idleTickCount, lastIdleTickCount);
        uint32_t totalTickIncrease = ComputeIncrease(totalTickCount, lastTotalTickCount);

        if (totalTickIncrease) {
            cpuLoad = 100 - ((idleTickIncrease * 100) / totalTickIncrease);
        }
        else {
            cpuLoad = 0;
        }

        lastIdleTickCount  = idleTickCount;
        lastTotalTickCount = totalTickCount;
    }

    uint32_t CpuStatistics::GetPercentageCpuLoad() const noexcept
    {
        return cpuLoad;
    }

    uint32_t CpuStatistics::ComputeIncrease(uint32_t currentCount, uint32_t lastCount) const
    {
        if (currentCount >= lastCount) {
            return currentCount - lastCount;
        }
        else {
            return std::numeric_limits<uint32_t>::max() - lastCount + currentCount;
        }
    }
} // namespace sys
