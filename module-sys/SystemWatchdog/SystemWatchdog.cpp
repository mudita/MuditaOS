// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <SystemWatchdog/SystemWatchdog.hpp>
#include "system/Common.hpp"
#include "critical.hpp"
#include "ticks.hpp"
#include "bsp/watchdog/watchdog.hpp"
#include <log/log.hpp>

namespace sys
{
    using namespace cpp_freertos;

    namespace
    {
        // Watchdog thread stack size
        constexpr std::uint16_t stackDepthWords = 256;
        // Timeout period for refresh - 90s
        constexpr TickType_t refreshTimeoutPeriod = pdMS_TO_TICKS(90000);
        // Timeout period for the actual watchdog (has to be longer than maximum allowed WFI time 60s) - 64s
        constexpr TickType_t watchdogTimeoutPeriod = pdMS_TO_TICKS(64000);
        // Period of actual watchdog refresh - 16s
        constexpr TickType_t checkPeriod = pdMS_TO_TICKS(16000);
        // Timeout period for watchdog thread closure - 2s
        constexpr TickType_t closurePeriod = pdMS_TO_TICKS(2000);
    } // namespace

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
#if (DISABLE_WDOG)
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
#if (!DISABLE_WDOG)
        // Critical section not required (atomic 32-bit writes)
        lastRefreshTimestamp = Ticks::GetTicks();
#endif
    }

    void SystemWatchdog::Run()
    {
        while (enableRunLoop) {
            Delay(checkPeriod);

            if (timeoutOccurred) {
                continue;
            }

            // Critical section not required (atomic 32-bit reads)
            if ((Ticks::GetTicks() - lastRefreshTimestamp) >= refreshTimeoutPeriod) {
                // Allow HW watchdog timeout to occur
                timeoutOccurred = true;
                LOG_FATAL("System watchdog timeout, system will be reset soon!");
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
#if (!DISABLE_WDOG)
        enableRunLoop = false;
        xTaskAbortDelay(GetHandle());
        if (!taskEndedSem.Take(closurePeriod)) {
            LOG_ERROR("Watchdog thread was not gently closed, killing");
        }
#endif
    }
} // namespace sys
