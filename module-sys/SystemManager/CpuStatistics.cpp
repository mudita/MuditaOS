// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/CpuStatistics.hpp>
#include <log/log.hpp>
#include <FreeRTOS.h>
#include <task.h>
#include <Utils.hpp>

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
        printer   = std::make_unique<cpu::stats::PackPrinter>();
#else
        // to change printer change assignment
        // printer = std::make_unique<cpu::stats::LogPrinter>();
        printer = std::make_unique<cpu::stats::NullPrinter>();
#endif
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
        if (ret.changed != sys::cpu::algorithm::Change::NoChange) {
            printer->printCPUChange(ret);
#if PROF_ON
            printer->printSysUsage(data, data_size);
#endif
        }
        printer->printPowerConsumption();
        StoreSysUsage();
    }

    void CpuStatistics::UpdatePercentageCpuLoad()
    {
        uint32_t idleTickCount     = xTaskGetIdleRunTimeCounter();
        uint32_t totalTickCount    = ulHighFrequencyTimerTicks();
        uint32_t idleTickIncrease  = utils::computeIncrease(idleTickCount, lastIdleTickCount);
        uint32_t totalTickIncrease = utils::computeIncrease(totalTickCount, lastTotalTickCount);
        lastIdleTickCount          = idleTickCount;
        lastTotalTickCount         = totalTickCount;
        if (totalTickIncrease != 0u) {
            cpuLoad = 100 - ((idleTickIncrease * 100) / totalTickIncrease);
        }
        else {
            cpuLoad = 0;
        }
    }

    uint32_t CpuStatistics::GetPercentageCpuLoad()
    {
        UpdatePercentageCpuLoad();
        return cpuLoad;
    }

} // namespace sys
