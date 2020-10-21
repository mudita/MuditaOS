// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <log/log.hpp>

#include "Service/Service.hpp"
#include "json/json11.hpp" // for Json
#include <string>          // for string

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

      public:
        MessageHandler(std::string &message, sys::Service *OwnerService);
        static xQueueHandle sendQueue;

        bool isJSONNull()
        {
            return messageJson.is_null();
        };
        bool isValid()
        {
            return JsonErrorMsg.empty();
        }
        std::string &getErrorString()
        {
            return JsonErrorMsg;
        };
        void processMessage();
        static void putToSendQueue(const std::string msg);

      private:
        json11::Json messageJson;
        std::string JsonErrorMsg;
        sys::Service *OwnerServicePtr = nullptr;
    };
} // namespace parserFSM
