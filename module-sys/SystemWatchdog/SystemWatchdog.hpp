// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/Service/Watchdog.hpp>
#include "thread.hpp"

namespace sys
{
    //
    // System watchdog
    //
    // System watchdog works in cooperation with an actual watchdog
    // (HW or simulated). After initialization, it expects to be continually
    // refreshed within a certain period of time.
    //
    // As long as its timing requirements are satisfied, it takes care
    // of refreshing the actual watchdog so a reset will not occur. Otherwise,
    // it will allow the actual watchdog to timeout causing a system reset.
    //
    class SystemWatchdog : public Watchdog, private cpp_freertos::Thread
    {
      public:
        static constexpr auto threadName = "SystemWatchdog";

        SystemWatchdog(const SystemWatchdog &) = delete;
        SystemWatchdog(SystemWatchdog &&)      = delete;
        SystemWatchdog &operator=(const SystemWatchdog &) = delete;
        SystemWatchdog &operator=(SystemWatchdog &&) = delete;

        static SystemWatchdog &getInstance();

        // Initialize the actual watchdog and start the thread
        bool init();

        void refresh() override;

      private:
        SystemWatchdog();

        // Timeout period for refresh
        static constexpr TickType_t refreshTimeoutPeriod = pdMS_TO_TICKS(90000);
        // Timeout period for the actual watchdog
        static constexpr TickType_t watchdogTimeoutPeriod = pdMS_TO_TICKS(16000);
        // Period of actual watchdog refresh
        static constexpr TickType_t checkPeriod = pdMS_TO_TICKS(8000);

        void Run() final;

        TickType_t lastRefreshTimestamp = 0;
        bool timeout_occurred           = false;

        static_assert(sizeof(lastRefreshTimestamp) == 4 && alignof(decltype(lastRefreshTimestamp)) == 4,
                      "SystemWatchdog::lastRefreshTimestamp must be 32-bit long and properly aligned otherwise data "
                      "races may occur");
    };

} // namespace sys
