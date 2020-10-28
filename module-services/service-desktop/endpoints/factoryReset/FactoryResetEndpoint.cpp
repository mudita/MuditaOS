// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FactoryResetEndpoint.hpp"

#include <Service/Bus.hpp> // for Bus
#include <memory>          // for make_shared

#include "DesktopMessages.hpp" // for FactoryMessage
#include "ServiceDesktop.hpp"  // for service_desktop
#include "Context.hpp"         // for Context
#include "MessageHandler.hpp"  // for MessageHandler
#include "json/json11.hpp"     // for Json, Json::object

auto FactoryResetEndpoint::handle(Context &context) -> void
{
    if (context.getMethod() == parserFSM::http::Method::post) {

        if (context.getBody()[parserFSM::json::factoryRequest] == true) {
            auto msg = std::make_shared<sdesktop::FactoryMessage>();
            sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);

            context.setResponseBody(json11::Json::object({{parserFSM::json::factoryRequest, true}}));
        }
        else {
            context.setResponseBody(json11::Json::object({{parserFSM::json::factoryRequest, false}}));
        }

        MessageHandler::putToSendQueue(context.createSimpleResponse());

        return;
    }
    else {
        context.setResponseBody(json11::Json::object({{parserFSM::json::factoryRequest, false}}));

        MessageHandler::putToSendQueue(context.createSimpleResponse());

        return;
    }
}
