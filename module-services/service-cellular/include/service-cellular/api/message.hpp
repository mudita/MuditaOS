// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

namespace cellular::msg
{
    struct Request : public sys::msg::Request
    {
        const char *target() const final
        {
            return m_target;
        }

      private:
        static constexpr const char *m_target = cellular::service::name;
    };

    struct Response : public sys::msg::Response
    {
        explicit Response(bool retCode = true) : sys::msg::Response(retCode)
        {}
    };

    struct Notification : public sys::msg::Notification
    {
        sys::BusChannel channel() const final
        {
            return m_channel;
        }

      private:
        static constexpr sys::BusChannel m_channel = sys::BusChannel::ServiceCellularNotifications;
    };

} // namespace cellular::msg
