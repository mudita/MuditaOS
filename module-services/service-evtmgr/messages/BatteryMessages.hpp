#pragma once

#include "Message.hpp"

namespace sevm
{
    class BatteryLevelMessage : public Message
    {
      public:
        BatteryLevelMessage() : Message(MessageType::EVMBatteryLevel)
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
} // namespace sevm
