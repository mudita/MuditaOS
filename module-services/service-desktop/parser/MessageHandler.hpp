// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <json11.hpp>
#include <log.hpp>

#include <string>

namespace sys
{
    class Service;
} // namespace sys

extern "C"
{
#include <FreeRTOS.h> // for xQueueHandle
#include <queue.h>
}

class EndpointFactory;

namespace parserFSM
{
    class MessageHandler
    {
        static xQueueHandle sendQueue;

        json11::Json messageJson;
        std::string JsonErrorMsg;
        sys::Service *OwnerServicePtr = nullptr;
        std::unique_ptr<EndpointFactory> endpointFactory;

      public:
        MessageHandler(sys::Service *OwnerService, std::unique_ptr<EndpointFactory> endpointFactory);

        [[nodiscard]] auto isJSONNull() const -> bool
        {
            return messageJson.is_null();
        };
        [[nodiscard]] auto isValid() const noexcept -> bool
        {
            return JsonErrorMsg.empty();
        }
        [[nodiscard]] auto getErrorString() const -> const std::string &
        {
            return JsonErrorMsg;
        };
        void parseMessage(const std::string &msg);
        void processMessage();
        static void putToSendQueue(const std::string &msg);
        static void setSendQueueHandle(xQueueHandle handle)
        {
            sendQueue = handle;
        }
    };
} // namespace parserFSM
