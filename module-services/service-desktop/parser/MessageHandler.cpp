// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessageHandler.hpp"

#include <bits/exception.h> // for exception
#include <inttypes.h>       // for PRIu32
#include <memory>           // for operator!=, unique_ptr

#include "Context.hpp"         // for Context
#include "EndpointFactory.hpp" // for EndpointFactory
#include "log/log.hpp"         // for LOG_ERROR, LOG_DEBUG
#include "Endpoint.hpp"        // for Endpoint
#include "FreeRTOS.h"          // for xQueueHandle

namespace sys
{
    class Service;
} // namespace sys

using namespace parserFSM;

xQueueHandle MessageHandler::sendQueue;

MessageHandler::MessageHandler(std::string &message, sys::Service *OwnerService) : OwnerServicePtr(OwnerService)
{
    try {
        messageJson = json11::Json::parse(message, JsonErrorMsg);
    }
    catch (const std::exception &e) {
        LOG_ERROR("Cannot create MessageHandler! err:%s", e.what());
    }
}

void MessageHandler::processMessage()
{
    Context context(messageJson);

    LOG_DEBUG("[MsgHandler]\nmethod: %d\nendpoint: %d\nuuid: %" PRIu32 "\nbody: %s\n",
              static_cast<int>(context.getMethod()),
              static_cast<int>(context.getEndpoint()),
              context.getUuid(),
              context.getBody().dump().c_str());

    auto handler = EndpointFactory::create(context, OwnerServicePtr);

    if (handler != nullptr) {
        handler->handle(context);
    }
    else {
        LOG_ERROR("No way to handle!");
    }
}

void MessageHandler::putToSendQueue(const std::string msg)
{
    if (uxQueueSpacesAvailable(sendQueue) != 0) {
        auto *responseString = new std::string(msg);
        xQueueSend(sendQueue, &responseString, portMAX_DELAY);
    }
}
