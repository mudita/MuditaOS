// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
    class BatterySetCriticalLevel : public Message
    {
      public:
        BatterySetCriticalLevel(std::uint8_t level)
            : Message(MessageType::EVMBatterySetCriticalLevel), criticalLevel(level)
        {}
        unsigned int criticalLevel = 0;
    };

    class BatteryLevelCriticalMessage : public Message
    {
      public:
        BatteryLevelCriticalMessage() : Message(MessageType::EVMBatteryLevelCritical)
        {}
    };

    class BatteryBrownoutMessage : public Message
    {
      public:
        BatteryBrownoutMessage() : Message(MessageType::EVMBatteryBrownout)
        {}
    };

} // namespace sevm
