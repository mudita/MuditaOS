﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateEndpoint.hpp"
#include "UpdateMuditaOS.hpp"

#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <endpoints/Context.hpp>
#include <endpoints/messages/MessageHelper.hpp>

#include <json/json11.hpp>
#include <purefs/filesystem_paths.hpp>
#include <vfs.hpp>

#include <filesystem>
#include <memory>

using namespace parserFSM;

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
    std::string cmd = context.getBody()[parserFSM::json::updateprocess::command].string_value();
    if (cmd == parserFSM::json::updateprocess::commands::abort) {
        auto owner        = static_cast<ServiceDesktop *>(ownerServicePtr);
        auto currentState = owner->updateOS->status;
        if (currentState <= updateos::UpdateState::ExtractingFiles) {
            owner->updateOS->setUpdateAbortFlag(true);
            context.setResponseBody(json11::Json::object({{parserFSM::json::updateprocess::updateAborted, true}}));
            context.setResponseStatus(http::Code::OK);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Success;
        }
        else {
            context.setResponseBody(json11::Json::object({{parserFSM::json::updateprocess::updateAborted, false}}));
            context.setResponseStatus(http::Code::NotAcceptable);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Failure;
        }
    }

    std::string fileName = context.getBody()["fileName"].string_value();
    auto path            = purefs::dir::getUpdatesOSPath() / fileName;
    auto fileExists      = std::filesystem::exists(path.c_str());
    if (fileExists) {
        context.setResponseBody(json11::Json::object({{parserFSM::json::updateReady, true}}));

        auto msg = std::make_shared<sdesktop::UpdateOsMessage>(fileName, context.getUuid());
        ownerServicePtr->bus.sendUnicast(msg, service::name::service_desktop);
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
    const auto updatesOSPath = purefs::dir::getUpdatesOSPath();
    json11::Json fileList    = vfs.listdir(updatesOSPath.c_str(), updateos::extension::update, true);

    context.setResponseBody(json11::Json::object{{parserFSM::json::updateFileList, fileList}});

    MessageHandler::putToSendQueue(context.createSimpleResponse());

    return sys::ReturnCodes::Success;
}
