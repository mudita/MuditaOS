// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CurrentMeasurementScope.hpp"

extern "C"
{
#include "FreeRTOS.h"
}
#include <timers.h>
#include <bsp/battery_charger/battery_charger.hpp>
#include <ticks.hpp>

#include <chrono>

namespace hal::battery::CurrentMeasurementScope
{
    namespace
    {
        using namespace std::chrono_literals;
        constexpr auto samplingTime = 100ms;
        TimerHandle_t samplingTimerHandle;

        static void getSample(TimerHandle_t xTimer)
        {
            LOG_DEBUG("[scope]: { \"timestamp\" : %ld, \"current\" : %d, \"current_filtered\" : %d  }",
                      cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()),
                      bsp::battery_charger::getCurrentMeasurement(),
                      bsp::battery_charger::getAvgCurrent());
        }
    } // namespace

    void start()
    {
        if (samplingTimerHandle == nullptr) {
            samplingTimerHandle =
                xTimerCreate("samplingTimer", pdMS_TO_TICKS(samplingTime.count()), true, nullptr, getSample);
        }
        xTimerStart(samplingTimerHandle, 0);
    }

} // namespace hal::battery::CurrentMeasurementScope
