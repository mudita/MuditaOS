// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemManager/CpuPrinter.hpp"
#include "SystemManager/SysCpuUpdateResult.hpp"
#include "SystemManager/SystemManagerCommon.hpp"
#include <battery_charger/battery_charger.hpp>
#include <time/time_conversion.hpp>
#include <RTOSWrapper/include/ticks.hpp>
#include <sink/RTTSink.hpp>
#include <msgpack11/msgpack11.hpp>

constexpr auto rtt_config = sink::rtt_traits{.chanel_no = 1, .size = 1024};

namespace sys::cpu::stats
{

    using namespace msgpack11;

    static auto getTimestamp()
    {
        return cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());
    }

    enum class PackID
    {
        Usage   = 1,
        ProcEnd = 2,
        Proc    = 3,
        Power   = 4,
    };

    PackPrinter::PackPrinter()
    {
        sink = std::make_unique<sink::RTT>(rtt_config);
    }

    PackPrinter::~PackPrinter()
    {}

    void PackPrinter::printSysUsage(struct task_prof_data *data, size_t size)
    {
        MsgPack procEnd = MsgPack::object({{"id", uint8_t(PackID::ProcEnd)}});
        auto ts         = getTimestamp();
        vTaskSuspendAll();
        {
            for (size_t i = 0; i < size; ++i) {
                if (data[i].exec_time == 0 && data[i].switches == 0) {
                    continue;
                }
                MsgPack obj = MsgPack::object{{"name", SystemManagerCommon::ServiceProcessor(data[i].task_TCB_id)},
                                              {"tcb", uint16_t(data[i].task_TCB_id)},
                                              {"time", data[i].exec_time},
                                              {"ts", ts},
                                              {"id", uint8_t(PackID::Proc)}};

                sink->put(obj.dump().c_str(), obj.dump().size());
            }
            sink->put(procEnd.dump().c_str(), procEnd.dump().size());
        }
        xTaskResumeAll();
    }

    void PackPrinter::printCPUChange(const cpu::UpdateResult &ret)
    {
        MsgPack obj = MsgPack::object{{"id", uint8_t(PackID::Usage)},
                                      {"freq", uint32_t(ret.frequencySet)},
                                      {"name", ret.data.name},
                                      {"reason", ret.data.reason},
                                      {"requested", uint32_t(ret.data.frequency)},
                                      {"avgA", int32_t(bsp::battery_charger::getAvgCurrent())},
                                      {"nowA", int32_t(bsp::battery_charger::getCurrentMeasurement())},
                                      {"ts", getTimestamp()}};

        sink->put(obj.dump().c_str(), obj.dump().size());
    }

    void PackPrinter::printPowerConsumption()
    {
        MsgPack obj = MsgPack::object{{"id", uint8_t(PackID::Power)},
                                      {"avgA", int32_t(bsp::battery_charger::getAvgCurrent())},
                                      {"nowA", int32_t(bsp::battery_charger::getCurrentMeasurement())},
                                      {"ts", getTimestamp()}};
        sink->put(obj.dump().c_str(), obj.dump().size());
    }
} // namespace sys::cpu::stats
