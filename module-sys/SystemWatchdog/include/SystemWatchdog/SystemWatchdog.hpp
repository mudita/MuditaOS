// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Watchdog.hpp"
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

        // It doesn't disable watchdog, but it simply close the petting thread
        void deinit();

      private:
        SystemWatchdog();

        // Timeout period for refresh
        static constexpr TickType_t refreshTimeoutPeriod = pdMS_TO_TICKS(90000);
        // Timeout period for the actual watchdog
        static constexpr TickType_t watchdogTimeoutPeriod = pdMS_TO_TICKS(16000);
        // Period of actual watchdog refresh
        static constexpr TickType_t checkPeriod = pdMS_TO_TICKS(8000);
        // Timeout period for watchdog thread closure
        static constexpr TickType_t closurePeriod = pdMS_TO_TICKS(2000);

        void Run() final;

        TickType_t lastRefreshTimestamp{0};
        bool timeout_occurred{false};
        bool enableRunLoop{false};
        cpp_freertos::BinarySemaphore taskEndedSem{false};

        static_assert(sizeof(lastRefreshTimestamp) == 4 && alignof(decltype(lastRefreshTimestamp)) == 4,
                      "SystemWatchdog::lastRefreshTimestamp must be 32-bit long and properly aligned otherwise data "
                      "races may occur");
    };

} // namespace sys
