// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RestoreEndpoint.hpp"
#include "DesktopMessages.hpp"
#include "ParserUtils.hpp"
#include "ServiceDesktop.hpp"
#include <Service/Bus.hpp>

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
