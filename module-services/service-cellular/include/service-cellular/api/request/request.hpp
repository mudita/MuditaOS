// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

#include <service-cellular/api/common.hpp>

namespace cellular::msg
{
    struct Request : public sys::Message
    {
        Request() : Message(Type::Data)
        {}
        static constexpr const char *target = cellular::service::name;
    };
    struct Response : public sys::Message
    {
        Response(bool retCode = true) : Message(Type::Data), retCode(retCode)
        {}
        bool retCode;
    };

} // namespace cellular::msg
