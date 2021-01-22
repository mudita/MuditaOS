// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RestoreEndpoint.hpp"

#include <endpoints/Context.hpp>
#include <endpoints/messages/MessageHelper.hpp>
#include <parser/ParserUtils.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <Service/Bus.hpp>
#include <json/json11.hpp>

#include <memory>

using namespace parserFSM;

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
