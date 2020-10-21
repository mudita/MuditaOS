// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateEndpoint.hpp"

#include "Context.hpp"         // for Context
#include "DesktopMessages.hpp" // for UpdateOsMessage
#include "MessageHandler.hpp"  // for MessageHandler
#include "ServiceDesktop.hpp"  // for service_desktop
#include "UpdatePureOS.hpp"    // for update
#include "vfs.hpp"             // for vfs, os_updates

#include <Service/Bus.hpp> // for Bus

#include "json/json11.hpp" // for Json, Json::object
#include <filesystem>      // for operator/, path
#include <memory>          // for allocator, make_shared

auto UpdateEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::post:
        run(context);
        break;
    case http::Method::get:
        getUpdates(context);
        break;
    default:
        break;
    }
}

auto UpdateEndpoint::run(Context &context) -> sys::ReturnCodes
{
    std::string fileName = context.getBody()["fileName"].string_value();
    auto path            = purefs::dir::os_updates / fileName;
    auto fileExists      = vfs.fileExists(path.c_str());
    if (fileExists) {
        context.setResponseBody(json11::Json::object({{parserFSM::json::updateReady, true}}));

        auto msg = std::make_shared<sdesktop::UpdateOsMessage>(fileName, context.getUuid());
        sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Success;
    }
    else {
        context.setResponseBody(json11::Json::object({{parserFSM::json::updateReady, false}}));
        context.setResponseStatus(http::Code::InternalServerError);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Failure;
    }
}

auto UpdateEndpoint::getUpdates(Context &context) -> sys::ReturnCodes
{
    json11::Json fileList = vfs.listdir(purefs::dir::os_updates.c_str(), updateos::extension::update, true);

    context.setResponseBody(json11::Json::object{{parserFSM::json::updateFileList, fileList}});

    MessageHandler::putToSendQueue(context.createSimpleResponse());

    return sys::ReturnCodes::Success;
}
