// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Message.hpp"

namespace sys
{
    class Timer; // Forward declaration

    class TimerMessage : public SystemMessage
    {
        timer::SystemTimer *timer = nullptr;

      public:
        explicit TimerMessage(timer::SystemTimer *timer)
            : SystemMessage(SystemMessageType::Timer, ServicePowerMode::Active), timer(timer)
        {}

        [[nodiscard]] auto getTimer() noexcept -> timer::SystemTimer *
        {
            return timer;
        }
    };
} // namespace sys
