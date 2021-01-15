﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Message.hpp"

#include <module-bsp/bsp/torch/torch.hpp>

namespace sevm
{
    class BatteryLevelMessage : public Message
    {
      public:
        BatteryLevelMessage(uint8_t levelPercents, bool fullyCharged)
            : Message(MessageType::EVMBatteryLevel), levelPercents(levelPercents), fullyCharged(fullyCharged)
        {
            type = Type::Data;
        }
        uint8_t levelPercents = 0;
        bool fullyCharged     = false;
    };

    class BatteryPlugMessage : public Message
    {
      public:
        BatteryPlugMessage() : Message(MessageType::EVMChargerPlugged)
        {
            type = Type::Data;
        }
        bool plugged = false;
    };
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
    {};

    class BatteryLevelNormalMessage : public sys::Message
    {};

    class BatteryBrownoutMessage : public sys::Message
    {};

} // namespace sevm
