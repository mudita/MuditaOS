// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateEndpoint.hpp"
#include "UpdateMuditaOS.hpp"

#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <endpoints/Context.hpp>
#include <service-appmgr/messages/PreventBlockingRequest.hpp>
#include <service-appmgr/model/ApplicationManager.hpp>

#include <json11.hpp>
#include <purefs/filesystem_paths.hpp>

#include <filesystem>
#include <memory>

using namespace parserFSM;

auto UpdateEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::post:
        preventBlockingDevice();
        run(context);
        break;
    case http::Method::get:
        preventBlockingDevice();
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
    std::error_code errorCode;
    struct DirectoryEntry
    {
        std::string fileName;
        uint32_t fileSize;
        json11::Json to_json() const
        {
            return (json11::Json::object{{"name", fileName}, {"size", std::to_string(fileSize)}});
        }
    };

    auto dirEntryVector = std::vector<DirectoryEntry>();
    for (const auto &p : fs::directory_iterator(updatesOSPath, errorCode)) {
        if (errorCode) {
            LOG_WARN("can't get directory contents for %s, \"%s\"", updatesOSPath.c_str(), errorCode.message().c_str());
            return sys::ReturnCodes::Failure;
        }

        if (!p.is_directory() && p.path().extension() == updateos::extension::update) {
            dirEntryVector.push_back(DirectoryEntry{p.path().string(), static_cast<uint32_t>(p.file_size())});
        }
    }

    json11::Json fileList = dirEntryVector;
    context.setResponseBody(json11::Json::object{{parserFSM::json::updateFileList, fileList}});

    MessageHandler::putToSendQueue(context.createSimpleResponse());

    return sys::ReturnCodes::Success;
}

void UpdateEndpoint::preventBlockingDevice()
{
    auto msg = std::make_shared<app::manager::PreventBlockingRequest>(service::name::service_desktop);
    ownerServicePtr->bus.sendUnicast(std::move(msg), app::manager::ApplicationManager::ServiceName);
}
