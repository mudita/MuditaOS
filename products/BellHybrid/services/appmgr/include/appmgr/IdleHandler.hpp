// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <Service/Message.hpp>
#include <Timers/TimerHandle.hpp>

namespace app::manager
{
    class IdleHandler
    {
      public:
        explicit IdleHandler(sys::Service *serv);

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
