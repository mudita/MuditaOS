// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Message.hpp"

#include <module-bsp/bsp/torch/torch.hpp>

namespace sevm
{
    class BatteryStatusChangeMessage : public sys::DataMessage
    {};

    class BatterySetCriticalLevel : public sys::DataMessage
    {
      public:
        BatterySetCriticalLevel(std::uint8_t level) : criticalLevel(level)
        {}
        const unsigned int criticalLevel = 0;
    };

    class BatteryStateChangeMessage : public sys::DataMessage
    {};

    class BatteryBrownoutMessage : public sys::DataMessage
    {};

    class USBPlugEvent : public sys::DataMessage
    {
      public:
        enum class Event
        {
            CablePlugged,
            CableUnplugged
        };

        explicit USBPlugEvent(const Event newEvent)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), event(newEvent){};

        Event event = Event::CableUnplugged;
    };
} // namespace sevm
