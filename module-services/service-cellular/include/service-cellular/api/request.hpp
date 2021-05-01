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

    namespace request
    {
        namespace sim
        {
            struct GetLockState : public msg::Request
            {
                struct Response : public msg::Response
                {
                    Response(bool locked) : locked(locked)
                    {}
                    bool locked;
                };
            };

            struct SetPin : msg::Request
            {
                SetPin(api::PassCodeType pcType, api::PassCode pc, api::PassCode pin)
                    : passCodeType(pcType), passCode(pc), pin(pin)
                {}
                api::PassCodeType passCodeType;
                api::PassCode passCode;
                api::PassCode pin;

                struct Response : msg::Response
                {
                    Response(bool retCode) : msg::Response(retCode)
                    {}
                };
            };
        } // namespace sim

    } // namespace request
} // namespace cellular::msg
