// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessageHandler.hpp"

#include <endpoints/Context.hpp>
#include <endpoints/EndpointFactory.hpp>

#include <FreeRTOS.h>
#include <log.hpp>

#include <bits/exception.h>
#include <memory>

namespace sys
{
    class Service;
} // namespace sys

using namespace parserFSM;

xQueueHandle MessageHandler::sendQueue;

MessageHandler::MessageHandler(sys::Service *OwnerService, std::unique_ptr<EndpointFactory> endpointFactory)
    : OwnerServicePtr(OwnerService), endpointFactory(std::move(endpointFactory))
{}

void MessageHandler::parseMessage(const std::string &msg)
{
    try {
        messageJson = json11::Json::parse(msg, JsonErrorMsg);
    }
    catch (const std::exception &e) {
        LOG_ERROR("Cannot create MessageHandler! err:%s", e.what());
    }
}

void MessageHandler::processMessage()
{
    auto context = ContextFactory::create(messageJson);

    LOG_DEBUG("[MsgHandler]\nmethod: %d\nendpoint: %d\nuuid: %d\n",
              static_cast<int>(context->getMethod()),
              static_cast<int>(context->getEndpoint()),
              context->getUuid());

    auto handler = endpointFactory->create(*context, OwnerServicePtr);

    if (handler != nullptr) {
        handler->handle(*context);
    }
    else {
        LOG_ERROR("No way to handle!");
    }
}

void MessageHandler::putToSendQueue(const std::string &msg)
{
    if (uxQueueSpacesAvailable(sendQueue) != 0) {
        auto responseString = new std::string(msg);
        xQueueSend(sendQueue, &responseString, portMAX_DELAY);
    }
}
