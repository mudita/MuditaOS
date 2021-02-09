// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateEndpoint.hpp"
#include "UpdateMuditaOS.hpp"

#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <endpoints/Context.hpp>
#include <endpoints/messages/MessageHelper.hpp>

#include <Service/Bus.hpp>
#include <json/json11.hpp>
#include <purefs/filesystem_paths.hpp>

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
    std::string fileName = context.getBody()["fileName"].string_value();
    auto path            = purefs::dir::getUpdatesOSPath() / fileName;
    auto fileExists      = std::filesystem::exists(path.c_str());
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
    const auto updatesOSPath = purefs::dir::getUpdatesOSPath();
    struct DirectoryEntry
    {
        std::string fileName;
        uint32_t fileSize;
        json11::Json to_json() const
        {
            return (json11::Json::object{{"name", fileName}, {"size", std::to_string(fileSize)}});
        }
    };
    LOG_DEBUG("iterating dir %s", updatesOSPath.string().c_str());
    auto dirEntryVector = std::vector<DirectoryEntry>();
    for (const auto &p : fs::directory_iterator(updatesOSPath)) {
        LOG_DEBUG("update entry test: %s", p.path().string().c_str());
        if (!p.is_directory() && p.path().extension() == updateos::extension::update) {
            dirEntryVector.push_back(DirectoryEntry{p.path().string(), static_cast<uint32_t>(p.file_size())});
            LOG_DEBUG("updat entry found: %s", p.path().string().c_str());
        }
    }

    json11::Json fileList = dirEntryVector;
    context.setResponseBody(json11::Json::object{{parserFSM::json::updateFileList, fileList}});

    MessageHandler::putToSendQueue(context.createSimpleResponse());

    return sys::ReturnCodes::Success;
}
