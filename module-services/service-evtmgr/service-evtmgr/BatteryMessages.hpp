// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Message.hpp"
#include <battery/BatteryState.hpp>

namespace sevm
{
    class BatteryStatusChangeMessage : public sys::DataMessage
    {};

    class BatteryStateChangeMessage : public sys::DataMessage
    {
      public:
        explicit BatteryStateChangeMessage(BatteryState::State state) : state{state} {};

        [[nodiscard]] BatteryState::State getState() const
        {
            return state;
        }

      private:
        BatteryState::State state{BatteryState::State::Normal};
    };

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
