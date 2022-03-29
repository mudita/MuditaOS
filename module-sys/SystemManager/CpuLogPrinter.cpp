// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemManager/CpuPrinter.hpp"
#include "SystemManager/SysCpuUpdateResult.hpp"
#include <SystemManager/SystemManagerCommon.hpp>
#include <log/log.hpp>

#include <cinttypes>

extern "C"
{
    uint32_t CLOCK_GetFreq(int);
}

namespace sys::cpu::stats
{

    void LogPrinter::printSysUsage(struct task_prof_data *data, size_t size)
    {
        vTaskSuspendAll();
        {
            for (size_t i = 0; i < size; ++i) {
                if (data[i].exec_time == 0 && data[i].switches == 0) {
                    continue;
                }

                LOG_PRINTF("%s,%" PRIu32 ",%" PRIu32 ",%" PRIu32 "\n",
                           SystemManagerCommon::ServiceProcessor(i).c_str(),
                           data[i].task_TCB_id,
                           data[i].exec_time,
                           data[i].switches);
                /// NOTE: version below is much lighter and doesn't need system suspend, it requires GDB to show what
                /// were the
                // task names LOG_PRINTF("%d,%" PRIu32 "\n", data[i].task_TCB_id, data[i].exec_time);
            }
        }
        xTaskResumeAll();
    }

    void LogPrinter::printCPUChange(const cpu::UpdateResult &ret)
    {
        LOG_PRINTF("CPU freq changed to: %d by: %s reason: %s for freq: %d curent: %" PRIu32 "\n",
                   int(ret.frequencySet),
                   ret.data.name.c_str(),
                   ret.data.reason.c_str(),
                   int(ret.data.frequency),
                   CLOCK_GetFreq(0));
    }

    void LogPrinter::printPowerConsumption()
    {}
} // namespace sys::cpu::stats
