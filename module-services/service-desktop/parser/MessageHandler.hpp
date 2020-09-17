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
