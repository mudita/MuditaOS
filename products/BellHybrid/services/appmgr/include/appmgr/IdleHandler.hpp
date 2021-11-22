// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <Service/Message.hpp>
#include <Timers/TimerHandle.hpp>

#include <chrono>

namespace app::manager
{
    constexpr auto idleReturnTimeout = std::chrono::seconds{180};
    using connectFunction            = std::function<bool(const std::type_info &, sys::MessageHandler)>;

    class IdleHandler
    {
      public:
        IdleHandler(sys::Service *serv);

      protected:
        void handleStartIdleTimer(sys::Message *request);
        void handleRestartIdleTimer(sys::Message *request);
        void handleStopIdleTimer(sys::Message *request);
        void idleTimerCallback();

      private:
        sys::Service *serv;
        sys::TimerHandle idleTimer;
    };
} // namespace app::manager
