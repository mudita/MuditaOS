// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Timers/TimerHandle.hpp>
#include <SystemManager/CpuSentinel.hpp>
#include <Service/Service.hpp>

namespace evm
{
    class UserActivityHandler
    {
      public:
        UserActivityHandler(std::shared_ptr<sys::CpuSentinel> cpuSentinel, sys::Service *parent);

        void handleUserInput();

      private:
        void onUserActivityTimeout();

        std::shared_ptr<sys::CpuSentinel> cpuSentinel;
        sys::TimerHandle activityTimer;
    };
} // namespace evm
