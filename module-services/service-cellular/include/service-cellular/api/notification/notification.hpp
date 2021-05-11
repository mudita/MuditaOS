// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

#include <service-cellular/api/common.hpp>

namespace cellular::msg
{
    struct Notification : public sys::Message
    {
        Notification() : Message(Type::Data)
        {}
    };

    namespace notification
    {
        struct SimReady : public msg::Notification
        {
            SimReady(bool ready) : ready(ready)
            {}
            bool ready;
        };

        struct UnhandledCME : public msg::Notification
        {
            UnhandledCME(api::SimSlot sim, unsigned int code) : sim(sim), code(code)
            {}
            api::SimSlot sim;
            unsigned int code;
        };
    } // namespace notification
} // namespace cellular::msg
