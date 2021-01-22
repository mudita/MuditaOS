// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BackupEndpoint.hpp"

#include <endpoints/Context.hpp>
#include <parser/MessageHandler.hpp>
#include <parser/ParserUtils.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <Service/Bus.hpp>
#include <json/json11.hpp>
#include <purefs/filesystem_paths.hpp>

#include <filesystem>
#include <memory>

static bool backupReady = false;

using namespace parserFSM;

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
        if (const auto backupOSPath = purefs::dir::getBackupOSPath(); std::filesystem::exists(backupOSPath.c_str())) {
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
