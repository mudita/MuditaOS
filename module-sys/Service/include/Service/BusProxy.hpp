// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Message.hpp"
#include "Watchdog.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace sys
{
    class Service; // Forward declaration
    class Bus;     // Forward declaration

    class BusProxy
    {
      public:
        static constexpr auto defaultTimeout = 5000U;

        ~BusProxy() noexcept;

        bool sendUnicast(std::shared_ptr<Message> message, const std::string &targetName);
        SendResult sendUnicastSync(std::shared_ptr<Message> message,
                                   const std::string &targetName,
                                   std::uint32_t timeout);
        void sendMulticast(std::shared_ptr<Message> message, BusChannel channel);
        void sendBroadcast(std::shared_ptr<Message> message);

        template <typename Msg, typename... Params> bool sendUnicast(Params &&...params)
        {
            static_assert(std::is_base_of<sys::msg::Request, Msg>::value,
                          "Only sys::msg::Request can be sent via Unicast<>");
            auto msg = std::make_shared<Msg>(std::forward<Params>(params)...);
            return sendUnicast(msg, msg->target());
        }

        template <typename Msg, typename... Params> void sendMulticast(Params &&...params)
        {
            static_assert(std::is_base_of<sys::msg::Notification, Msg>::value,
                          "Only sys::msg::Notification can be sent via Multicast<>");
            auto msg = std::make_shared<Msg>(std::forward<Params>(params)...);
            sendMulticast(msg, msg->channel());
        }

        std::vector<BusChannel> channels;

        void sendResponse(std::shared_ptr<Message> response, std::shared_ptr<Message> request);

      private:
        friend class Service;
        explicit BusProxy(Service *owner, Watchdog &watchdog);

        void connect();
        void disconnect();

        Service *owner;
        Watchdog &watchdog;
        std::unique_ptr<Bus> busImpl;
    };
} // namespace sys
