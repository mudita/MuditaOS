// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <json/json11.hpp>
#include <log/log.hpp>

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

namespace parserFSM
{
    class MessageHandler
    {
        static xQueueHandle sendQueue;

        json11::Json messageJson;
        std::string JsonErrorMsg;
        sys::Service *OwnerServicePtr = nullptr;

      public:
        MessageHandler(const std::string &message, sys::Service *OwnerService);

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
        void processMessage();
        static void putToSendQueue(const std::string &msg);
        static void setSendQueueHandle(xQueueHandle handle)
        {
            sendQueue = handle;
        }
    };
} // namespace parserFSM
