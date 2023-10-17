// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "software_watchdog.hpp"
#include <critical.hpp>
#include <ticks.hpp>
#include <system/Common.hpp>
#include <log/log.hpp>
#include <cstdlib>

namespace bsp::watchdog
{
    using namespace cpp_freertos;

    static constexpr uint16_t stackDepthWords = 256;

    SoftwareWatchdog::SoftwareWatchdog()
        : Thread("SW_Watchdog", stackDepthWords, static_cast<UBaseType_t>(sys::ServicePriority::Realtime))
    {}

    bool SoftwareWatchdog::init(TickType_t timeoutPeriodMs)
    {
#ifdef DISABLE_WATCHDOG
        return true;
#else
        timeoutPeriod      = pdMS_TO_TICKS(timeoutPeriodMs);
        const auto started = Start();

        if (started) {
            refresh();
        }

        return started;
#endif
    }

    void SoftwareWatchdog::refresh()
    {
#ifndef DISABLE_WATCHDOG
        // Critical section not required (atomic 32-bit writes)
        lastRefreshTimestamp = Ticks::GetTicks();
#endif
    }

    void SoftwareWatchdog::Run()
    {
        TickType_t lastTimeoutTimestamp = xTaskGetTickCount();

        while (true) {
            vTaskDelayUntil(&lastTimeoutTimestamp, timeoutPeriod);

            // Critical section not required (atomic 32-bit reads)
            if (lastTimeoutTimestamp - lastRefreshTimestamp >= timeoutPeriod) {
                LOG_FATAL("!!! Software watchdog timeout, exiting !!!");
                exit(EXIT_FAILURE);
            }
        }
    }
} // namespace bsp::watchdog
