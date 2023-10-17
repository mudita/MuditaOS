// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CurrentMeasurementScope.hpp"

#include "FreeRTOS.h"
#include <timers.h>
#include <battery_charger/battery_charger.hpp>
#include <log/log.hpp>
#include <ticks.hpp>

#include <chrono>

namespace hal::battery::CurrentMeasurementScope
{
    namespace
    {
        using namespace std::chrono_literals;
        constexpr auto samplingTime = 100ms;
        TimerHandle_t samplingTimerHandle;

        void getSample([[maybe_unused]] TimerHandle_t xTimer)
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
            samplingTimerHandle = xTimerCreate(
                "CurrentMeasurementSamplingTimer", pdMS_TO_TICKS(samplingTime.count()), pdTRUE, nullptr, getSample);
        }
        xTimerStart(samplingTimerHandle, 0);
    }
} // namespace hal::battery::CurrentMeasurementScope
