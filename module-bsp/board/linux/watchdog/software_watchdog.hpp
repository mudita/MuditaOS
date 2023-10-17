// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-os/RTOSWrapper/include/thread.hpp>

namespace bsp::watchdog
{
    class SoftwareWatchdog : private cpp_freertos::Thread
    {
      public:
        SoftwareWatchdog();
        SoftwareWatchdog(const SoftwareWatchdog &) = delete;
        SoftwareWatchdog(SoftwareWatchdog &&)      = delete;
        SoftwareWatchdog &operator=(const SoftwareWatchdog &) = delete;
        SoftwareWatchdog &operator=(SoftwareWatchdog &&) = delete;

        bool init(TickType_t timeoutPeriodMs);
        void refresh();

      private:
        void Run() final;

        TickType_t timeoutPeriod        = 0;
        TickType_t lastRefreshTimestamp = 0;
    };
} // namespace bsp::watchdog
