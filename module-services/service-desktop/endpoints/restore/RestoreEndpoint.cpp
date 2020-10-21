// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RestoreEndpoint.hpp"

#include "Context.hpp"         // for Context
#include "DesktopMessages.hpp" // for RestoreMessage
#include "MessageHandler.hpp"  // for MessageHandler
#include "ParserUtils.hpp"     // for restoreRequest, Method, Method::post
#include "ServiceDesktop.hpp"  // for service_desktop

#include <Service/Bus.hpp> // for Bus

#include "json/json11.hpp" // for Json, Json::object
#include <memory>          // for make_shared

auto RestoreEndpoint::handle(Context &context) -> void
{
    if (context.getMethod() == parserFSM::http::Method::post) {

        if (context.getBody()[parserFSM::json::restoreRequest] == true) {
            auto msg = std::make_shared<sdesktop::RestoreMessage>();
            sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);

            context.setResponseBody(json11::Json::object({{parserFSM::json::restoreRequest, true}}));
        }
        else {
            context.setResponseBody(json11::Json::object({{parserFSM::json::restoreRequest, false}}));
        }

        MessageHandler::putToSendQueue(context.createSimpleResponse());

        return;
    }
    else {
        return;
    }
}
