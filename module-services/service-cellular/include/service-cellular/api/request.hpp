// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

#include <service-cellular/api/common.hpp>

namespace cellular::msg
{
    struct Request : public sys::DataMessage
    {
        static constexpr const char *target = cellular::service::name;
    };
    struct Response : public sys::DataMessage
    {};

    namespace request
    {
        namespace sim
        {
            struct GetLockState : public cellular::msg::Request
            {
                struct Response : public cellular::msg::Response
                {
                    Response(bool locked) : locked(locked)
                    {}
                    bool locked;
                };
            };
        } // namespace sim

    } // namespace request
} // namespace cellular::msg
