// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FilesystemEndpoint.hpp"
#include "service-desktop/DesktopMessages.hpp"
#include "service-desktop/ServiceDesktop.hpp"
#include <purefs/filesystem_paths.hpp>

using namespace parserFSM;

auto FilesystemEndpoint::handle(Context &context) -> void
{
    LOG_DEBUG("handle");
    switch (context.getMethod()) {
    case http::Method::post:
        run(context);
        break;
    default:
        break;
    }
}
static bool isWritable(const fs::path &file)
{
    auto lamb = [](std::FILE *stream) { std::fclose(stream); };

    std::unique_ptr<std::FILE, decltype(lamb)> sf(std::fopen(file.c_str(), "w"), lamb);

    return static_cast<bool>(sf);
}

auto FilesystemEndpoint::run(Context &context) -> sys::ReturnCodes
{
    LOG_DEBUG("running");
    sys::ReturnCodes returnCode = sys::ReturnCodes::Failure;
    std::string cmd             = context.getBody()[parserFSM::json::filesystem::command].string_value();

    context.setResponseBody(
        json11::Json::object({{json::status, std::to_string(static_cast<int>(sys::ReturnCodes::Failure))}}));

    auto owner = static_cast<ServiceDesktop *>(ownerServicePtr);

    if (cmd == parserFSM::json::filesystem::commands::download) {
        fs::path filePath    = context.getBody()[parserFSM::json::fileName].string_value();
        fs::path tmpFilePath = purefs::dir::getUpdatesOSPath() / filePath;

        uint32_t fileSize = context.getBody()[parserFSM::json::fileSize].int_value();

        LOG_DEBUG("got owner, file %s", tmpFilePath.c_str());

        if (isWritable(tmpFilePath)) {
            LOG_INFO("download %" PRIu32 " bytes to: %s", fileSize, tmpFilePath.c_str());

            if (owner->desktopWorker->startDownload(tmpFilePath, fileSize) == sys::ReturnCodes::Success) {
                context.setResponseStatus(parserFSM::http::Code::Accepted);
                returnCode = sys::ReturnCodes::Success;
            }
        }
        else {
            LOG_ERROR("download command failed, can't write %" PRIu32 " bytes to: %s", fileSize, tmpFilePath.c_str());
        }
    }
    else if (cmd == parserFSM::json::filesystem::commands::checkFile) {
        fs::path filePath = context.getBody()[parserFSM::json::fileName].string_value();
        LOG_DEBUG("Checking file: %s", filePath.c_str());

        context.setResponseBody(json11::Json::object{{json::fileExists, std::filesystem::exists(filePath)}});
        returnCode = sys::ReturnCodes::Success;
    }
    else {
        LOG_ERROR("unknown command: %s", cmd.c_str());
    }

    MessageHandler::putToSendQueue(context.createSimpleResponse());
    return returnCode;
}
