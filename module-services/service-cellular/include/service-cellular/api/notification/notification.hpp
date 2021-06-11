// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/api/common.hpp>
#include <service-cellular/api/message.hpp>

namespace cellular::msg::notification
{
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

    struct ModemStateChanged : public msg::Notification
    {
        explicit ModemStateChanged(api::ModemState state) : state(state)
        {}
        const api::ModemState state;
    };

    struct SimStateChanged : public msg::Notification
    {
        explicit SimStateChanged(api::SimState state) : state(state)
        {}
        const api::SimState state;
    };

} // namespace cellular::msg::notification
