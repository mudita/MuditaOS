// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BackupEndpoint.hpp"

#include <endpoints/Context.hpp>
#include <parser/MessageHandler.hpp>
#include <parser/ParserUtils.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <json11.hpp>
#include <purefs/filesystem_paths.hpp>

#include <filesystem>
#include <memory>

using namespace parserFSM;

auto BackupEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::get:
        request(context);
        break;
    case http::Method::post:
    case http::Method::put:
    case http::Method::del:
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        break;
    }
}
auto BackupEndpoint::request(Context &context) -> sys::ReturnCodes
{
    json11::Json responseBodyJson;
    auto owner = static_cast<ServiceDesktop *>(ownerServicePtr);

    if (context.getBody()[json::task].is_string()) {
        if (owner->getBackupRestoreStatus().task == context.getBody()[json::task].string_value()) {
            if (owner->getBackupRestoreStatus().state != ServiceDesktop::OperationState::Running) {
                context.setResponseStatus(parserFSM::http::Code::SeeOther);
            }

            context.setResponseBody(owner->getBackupRestoreStatus());
        }
        else {
            context.setResponseStatus(parserFSM::http::Code::NotFound);
        }
    }
    else if (context.getBody()[json::request] == true) {
        if (owner->getBackupRestoreStatus().state == ServiceDesktop::OperationState::Running) {
            // a backup is already running, don't start a second task
            context.setResponseStatus(parserFSM::http::Code::NotAcceptable);
        }
        else {
            // initialize new backup information
            owner->prepareBackupData();

            // start the backup process in the background
            ownerServicePtr->bus.sendUnicast(std::make_shared<sdesktop::BackupMessage>(),
                                             service::name::service_desktop);

            // return new generated backup info
            context.setResponseBody(owner->getBackupRestoreStatus());
        }
    }
    else {
        // unknown request for backup endpoint
        context.setResponseStatus(parserFSM::http::Code::BadRequest);
    }

    LOG_DEBUG("responding");
    MessageHandler::putToSendQueue(context.createSimpleResponse());

    return sys::ReturnCodes::Success;
}
