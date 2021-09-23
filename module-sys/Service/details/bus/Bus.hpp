// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "system/Common.hpp"
#include "Service/Message.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace sys
{
    class Service; // forward declaration

    class Bus
    {
        // Because of incorrect linking of CMake targets Bus methods are accessible from any point of the project.
        // In order to fix this, only BusProxy is able to access the methods of the Bus class.
        friend class BusProxy;

      private:
        /**
         * Sends a message directly to the specified target service.
         * @param message       Message to be sent
         * @param targetName    Target service
         * @param sender        Sender context
         * @return true on success, false otherwise
         */
        bool SendUnicast(std::shared_ptr<Message> message, const std::string &targetName, Service *sender);

        /**
         * Sends a message directly to the specified target service with timeout.
         * @param message       Message to be sent
         * @param targetName    Target service
         * @param sender        Sender context
         * @param timeout       Timeout
         * @return Return code and a response.
         */
        SendResult SendUnicastSync(std::shared_ptr<Message> message,
                                   const std::string &targetName,
                                   Service *sender,
                                   std::uint32_t timeout);

        /**
         * Sends a message to the specified channel.
         * @param message   Message to be sent
         * @param channel   Target channel
         * @param sender    Sender context
         */
        void SendMulticast(std::shared_ptr<Message> message, BusChannel channel, Service *sender);

        /**
         * Sends a message to all registered services.
         * @param message   Message to be sent
         * @param sender    Sender context
         */
        void SendBroadcast(std::shared_ptr<Message> message, Service *sender);

        /**
         * Sends a response to the specified request.
         * @param response  Response to be sent
         * @param request   Request
         * @param sender    Sender context
         */
        void SendResponse(std::shared_ptr<Message> response, std::shared_ptr<Message> request, Service *sender);

        /**
         * Registers a service.
         * @param service   Service to be registered
         */
        static void Add(Service *service);

        /**
         * Unregisters a service.
         * @param service   Service to be unregistered
         */
        static void Remove(Service *service);
    };
} // namespace sys
