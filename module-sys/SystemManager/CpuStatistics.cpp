// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/CpuStatistics.hpp>
#include <log/log.hpp>
#include <FreeRTOS.h>
#include <task.h>
#include <limits>

extern "C"
{
#include "prof.h"
}

namespace sys
{

    CpuStatistics::CpuStatistics()
    {

#if PROF_ON
        data_size = prof_pool_get_data().size;
        data      = new task_prof_data[data_size];
#endif
        printer = std::make_unique<cpu::stats::LogPrinter>();
    }

    CpuStatistics::~CpuStatistics()
    {
#if PROF_ON
        delete[] data;
#endif
    }

    void CpuStatistics::StoreSysUsage()
    {
#if PROF_ON
        vTaskSuspendAll();
        {
            if (auto ovf = prof_pool_flush(data, data_size); ovf != 0) {
                LOG_FATAL("prof pool flush overflow: %d", int(ovf));
            }
        }
        xTaskResumeAll();
#endif
    }

    void CpuStatistics::TrackChange(const cpu::UpdateResult &ret)
    {
        if (ret.changed) {
            printer->printCPUChange(ret);
#if PROF_ON
            printer->printSysUsage(data, data_size);
#endif
        }
        StoreSysUsage();
    }

    void CpuStatistics::UpdatePercentageCpuLoad()
    {
        uint32_t idleTickCount  = xTaskGetIdleRunTimeCounter();
        uint32_t totalTickCount = ulHighFrequencyTimerTicks();
        uint32_t idleTickIncrease  = ComputeIncrease(idleTickCount, lastIdleTickCount);
        uint32_t totalTickIncrease = ComputeIncrease(totalTickCount, lastTotalTickCount);
        lastIdleTickCount          = idleTickCount;
        lastTotalTickCount         = totalTickCount;
        if (totalTickIncrease != 0u) {
            cpuLoad = 100 - ((idleTickIncrease * 100) / totalTickIncrease);
        }
        else {
            cpuLoad = 0;
        }
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
