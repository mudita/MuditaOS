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
            explicit SimReady(bool ready) : ready(ready)
            {}
            const bool ready;
        };

        struct SimNeedPin : public msg::Notification
        {
            explicit SimNeedPin(unsigned int attempts) : attempts(attempts)
            {}
            const unsigned int attempts;
        };

        struct SimNeedPuk : public msg::Notification
        {
            explicit SimNeedPuk(unsigned int attempts) : attempts(attempts)
            {}
            const unsigned int attempts;
        };

        struct SimBlocked : public msg::Notification
        {};

        struct UnhandledCME : public msg::Notification
        {
            explicit UnhandledCME(unsigned int code) : code(code)
            {}
            const unsigned int code;
        };
    } // namespace notification
} // namespace cellular::msg
