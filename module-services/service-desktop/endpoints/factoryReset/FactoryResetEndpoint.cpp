// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FactoryResetEndpoint.hpp"

#include <endpoints/Context.hpp>
#include <parser/MessageHandler.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <json11.hpp>

#include <memory>

auto FactoryResetEndpoint::handle(parserFSM::Context &context) -> void
{
    if (context.getMethod() == parserFSM::http::Method::post) {

        if (context.getBody()[parserFSM::json::factoryRequest] == true) {
            auto msg = std::make_shared<sdesktop::FactoryMessage>();
            ownerServicePtr->bus.sendUnicast(msg, service::name::service_desktop);

            context.setResponseBody(json11::Json::object({{parserFSM::json::factoryRequest, true}}));
        }
        else {
            context.setResponseBody(json11::Json::object({{parserFSM::json::factoryRequest, false}}));
        }

        parserFSM::MessageHandler::putToSendQueue(context.createSimpleResponse());

        return;
    }
    else {
        context.setResponseBody(json11::Json::object({{parserFSM::json::factoryRequest, false}}));

        parserFSM::MessageHandler::putToSendQueue(context.createSimpleResponse());

        return;
    }
}
