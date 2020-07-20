#pragma once
#include "Service/Service.hpp"
#include <string>
#include "json/json11.hpp"
#include <log/log.hpp>
extern "C"
{
#include <FreeRTOS.h>
#include <queue.h>
}

namespace ParserStateMachine
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
        std::string &getErrorString()
        {
            return JsonErrorMsg;
        };
        void processMessage();

      private:
        json11::Json messageJson;
        std::string JsonErrorMsg;
        sys::Service *OwnerServicePtr = nullptr;

        void putToSendQueue(std::string msg);
    };
} // namespace ParserStateMachine
