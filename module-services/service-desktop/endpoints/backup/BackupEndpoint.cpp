// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BackupEndpoint.hpp"

#include <Service/Bus.hpp> // for Bus
#include <filesystem>      // for path
#include <memory>          // for make_shared

#include "DesktopMessages.hpp" // for BackupMessage
#include "ParserUtils.hpp" // for Method, backupReady, backupRequest, backupUpload, Code, Code::BadRequest, Method::del, Method::get, Method::post, Method::put
#include "ServiceDesktop.hpp" // for service_desktop
#include "Context.hpp"        // for Context
#include "MessageHandler.hpp" // for MessageHandler
#include "json/json11.hpp"    // for Json, Json::object
#include "vfs.hpp"            // for vfs, os_backup

static bool backupReady = false;

auto BackupEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::post:
        upload(context);
        break;
    case http::Method::get:
        request(context);
        break;
    case http::Method::put:
        [[fallthrough]];
    case http::Method::del:
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        break;
    }
}
auto BackupEndpoint::request(Context &context) -> sys::ReturnCodes
{
    json11::Json responseBodyJson;
    if (context.getBody()[json::backupReady] == true) {
        context.setResponseBody(json11::Json::object({{json::backupReady, backupReady}}));
    }
    else if (context.getBody()[json::backupRequest] == true) {
        auto msg = std::make_shared<sdesktop::BackupMessage>();
        sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);
        backupReady = true;

        context.setResponseBody(json11::Json::object({{json::backupRequest, true}}));
    }
    else {
        context.setResponseBody(json11::Json::object({{json::backupRequest, false}}));
    }

    MessageHandler::putToSendQueue(context.createSimpleResponse());

    return sys::ReturnCodes::Success;
}

auto BackupEndpoint::upload(Context &context) -> sys::ReturnCodes
{
    if (context.getBody()[json::backupUpload] == true) {
        if (vfs.fileExists(purefs::dir::os_backup.c_str())) {
            context.setResponseBody(json11::Json::object({{json::backupUpload, true}}));
        }
        else {
            context.setResponseBody(json11::Json::object({{json::backupUpload, false}}));
        }
    }
    else {
        context.setResponseBody(json11::Json::object({{json::backupReady, false}}));
    }

    MessageHandler::putToSendQueue(context.createSimpleResponse());

    return sys::ReturnCodes::Success;
}
