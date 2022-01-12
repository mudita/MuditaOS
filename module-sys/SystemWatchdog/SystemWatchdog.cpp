// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemWatchdog/SystemWatchdog.hpp>
#include "system/Common.hpp"
#include "critical.hpp"
#include "ticks.hpp"
#include "bsp/watchdog/watchdog.hpp"
#include <log/log.hpp>

namespace sys
{
    using namespace cpp_freertos;

    static constexpr uint16_t stackDepthWords = 256;

    SystemWatchdog::SystemWatchdog()
        : Thread(threadName, stackDepthWords, static_cast<UBaseType_t>(ServicePriority::High))
    {}

    SystemWatchdog &SystemWatchdog::getInstance()
    {
        static SystemWatchdog watchdog;
        return watchdog;
    }

    bool SystemWatchdog::init()
    {
#ifdef DISABLE_WATCHDOG
        return true;
#else
        if (!bsp::watchdog::init(watchdogTimeoutPeriod)) {
            return false;
        }
        bsp::watchdog::refresh();

        enableRunLoop = true;
        if (!Start()) {
            return false;
        }

        refresh();

        return true;
#endif
    }

    void SystemWatchdog::refresh()
    {
#ifndef DISABLE_WATCHDOG
        // Critical section not required (atomic 32-bit writes)
        lastRefreshTimestamp = Ticks::GetTicks();
#endif
    }

    void SystemWatchdog::Run()
    {
        while (enableRunLoop) {
            Delay(checkPeriod);

            if (timeout_occurred) {
                continue;
            }

            // Critical section not required (atomic 32-bit reads)
            if (Ticks::GetTicks() - lastRefreshTimestamp >= refreshTimeoutPeriod) {
                // Allow HW watchdog timeout to occur
                timeout_occurred = true;
                LOG_FATAL("!!! System watchdog timeout, system will be resetted soon !!!");
            }
            else {
                bsp::watchdog::refresh();
            }
        }

        // notify caller of deinit()
        taskEndedSem.Give();
    }

    void SystemWatchdog::deinit()
    {
#ifndef DISABLE_WATCHDOG
        enableRunLoop = false;
        xTaskAbortDelay(GetHandle());
        if (!taskEndedSem.Take(closurePeriod)) {
            LOG_ERROR("Watchdog thread was not gently closed, killing");
        }
#endif
    }
} // namespace sys
