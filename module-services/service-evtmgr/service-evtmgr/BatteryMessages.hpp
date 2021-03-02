// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Message.hpp"

#include <module-bsp/bsp/torch/torch.hpp>

namespace sevm
{
    class BatteryStatusChangeMessage : public sys::Message
    {};

    class BatterySetCriticalLevel : public sys::Message
    {
      public:
        BatterySetCriticalLevel(std::uint8_t level) : Message(), criticalLevel(level)
        {}
        unsigned int criticalLevel = 0;
    };

    class BatteryLevelCriticalCheckMessage : public sys::Message
    {};
    class BatteryLevelCriticalMessage : public sys::Message
    {
      public:
        explicit BatteryLevelCriticalMessage(bool charging) : charging(charging)
        {}

        bool isCharging() const noexcept
        {
            return charging;
        }

      private:
        bool charging;
    };

    class BatteryShutdownLevelMessage : public sys::Message
    {};

    class BatteryLevelNormalMessage : public sys::Message
    {};

    class BatteryBrownoutMessage : public sys::Message
    {};

} // namespace sevm
