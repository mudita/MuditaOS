// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessageHandler.hpp"
#include <endpoints/EndpointFactory.hpp>

#include <log/log.hpp>

#include <memory>

namespace sdesktop::endpoints
{

    MessageHandler::MessageHandler(sys::Service *OwnerService,
                                   std::function<void()> messageProcessedCallback,
                                   std::unique_ptr<EndpointFactory> endpointFactory)
        : messageProcessedCallback(std::move(messageProcessedCallback)), OwnerServicePtr(OwnerService),
          endpointFactory(std::move(endpointFactory))
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

        if (handler == nullptr) {
            LOG_ERROR("No way to handle!");
            return;
        }
        handler->handle(*context);
        messageProcessedCallback();
    }

} // namespace sdesktop::endpoints
