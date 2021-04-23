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

        std::vector<BusChannel> channels;

      private:
        friend class Service;
        explicit BusProxy(Service *owner, Watchdog &watchdog);

        void sendResponse(std::shared_ptr<Message> response, std::shared_ptr<Message> request);
        void connect();
        void disconnect();

        Service *owner;
        Watchdog &watchdog;
        std::unique_ptr<Bus> busImpl;
    };
} // namespace sys
