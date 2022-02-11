// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemManager/CpuPrinter.hpp"
#include "SystemManager/SysCpuUpdateResult.hpp"
#include <SystemManager/SystemManagerCommon.hpp>
#include "third-party/msgpack11/msgpack11/msgpack11.hpp"

namespace sys::cpu::stats
{

    using namespace msgpack11;
    enum class PackID
    {
        Proc,
        Usage,
    };

    void PackPrinter::printSysUsage(struct task_prof_data *data, size_t size)
    {
        vTaskSuspendAll();
        {
            for (size_t i = 0; i < size; ++i) {
                if (data[i].exec_time == 0 && data[i].switches == 0) {
                    continue;
                }
                MsgPack obj = MsgPack::object{{"id", uint32_t(PackID::Proc)},
                                              {"name", SystemManagerCommon::ServiceProcessor(i)},
                                              {"tcb", uint32_t(data[i].task_TCB_id)},
                                              {"t", data[i].exec_time}};
                LOG_PRINTF("%c%s\n", 2, obj.dump().c_str());
            }
        }
        xTaskResumeAll();
    }

    void PackPrinter::printCPUChange(const cpu::UpdateResult &ret)
    {
        MsgPack obj = MsgPack::object{{"id", uint32_t(PackID::Usage)},
                                      {"freq", uint32_t(ret.frequencySet)},
                                      {"name", ret.data.name},
                                      {"reason", ret.data.reason},
                                      {"requested", uint32_t(ret.data.frequency)}};
        LOG_PRINTF("%c%s\n", 2, obj.dump().c_str());
    }
} // namespace sys::cpu::stats
