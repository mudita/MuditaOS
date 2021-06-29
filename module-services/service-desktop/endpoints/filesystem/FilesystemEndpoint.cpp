// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FilesystemEndpoint.hpp"
#include "FileOperations.hpp"
#include "service-desktop/DesktopMessages.hpp"
#include "service-desktop/ServiceDesktop.hpp"
#include <purefs/filesystem_paths.hpp>
#include <filesystem>

using namespace parserFSM;

auto FilesystemEndpoint::handle(Context &context) -> void
{
    auto returnCode = sys::ReturnCodes::Success;
    switch (context.getMethod()) {
    case http::Method::get:
        returnCode = runGet(context);
        break;
    case http::Method::post:
        returnCode = runPost(context);
        break;
    default:
        LOG_ERROR("Unhandled method request: %u", static_cast<unsigned>(context.getMethod()));
        returnCode = sys::ReturnCodes::Failure;
        break;
    }
    LOG_DEBUG("returnCode: %u", static_cast<unsigned>(returnCode));
}
static bool isWritable(const fs::path &file)
{
    auto lamb = [](std::FILE *stream) { std::fclose(stream); };

    std::unique_ptr<std::FILE, decltype(lamb)> sf(std::fopen(file.c_str(), "w"), lamb);

    return static_cast<bool>(sf);
}

auto FilesystemEndpoint::startGetFile(Context &context) const -> sys::ReturnCodes
{
    std::filesystem::path filePath = context.getBody()[parserFSM::json::fileName].string_value();

    if (!std::filesystem::exists(filePath)) {
        LOG_ERROR("file not found: %s", filePath.c_str());

        context.setResponseStatus(parserFSM::http::Code::NotFound);
        context.setResponseBody(json11::Json::object({{json::reason, json::filesystem::reasons::fileDoesNotExist}}));
        return sys::ReturnCodes::Failure;
    }

    LOG_DEBUG("Checking file: %s", filePath.c_str());

    auto [rxID, fileSize] = fileOps.createReceiveIDForFile(filePath);

    if (!fileSize) {
        LOG_ERROR("File %s corrupted", filePath.c_str());

        context.setResponseStatus(parserFSM::http::Code::InternalServerError);
        context.setResponseBody(json11::Json::object({{json::reason, json::filesystem::reasons::fileDoesNotExist}}));
        return sys::ReturnCodes::Failure;
    }

    context.setResponseStatus(parserFSM::http::Code::OK);
    context.setResponseBody(
        json11::Json::object({{json::filesystem::rxID, static_cast<int>(rxID)},
                              {json::fileSize, static_cast<int>(fileSize)},
                              {json::filesystem::chunkSize, static_cast<int>(FileOperations::ChunkSize)}}));

    return sys::ReturnCodes::Success;
}

auto FilesystemEndpoint::getFileChunk(Context &context) const -> sys::ReturnCodes
{
    auto returnCode    = sys::ReturnCodes::Failure;
    const auto rxID    = context.getBody()[parserFSM::json::filesystem::rxID].int_value();
    const auto chunkNo = context.getBody()[parserFSM::json::filesystem::chunkNo].int_value();

    const auto data = fileOps.getDataForReceiveID(rxID, chunkNo);

    if (data.length()) {
        context.setResponseStatus(parserFSM::http::Code::OK);
        context.setResponseBody(json11::Json::object({{json::filesystem::rxID, static_cast<int>(rxID)},
                                                      {json::filesystem::chunkNo, static_cast<int>(chunkNo)},
                                                      {json::filesystem::data, data}}));

        returnCode = sys::ReturnCodes::Success;
    }
    else {
        std::ostringstream errorReason;
        errorReason << "Invalid request rxID: " << std::to_string(rxID) << ", chunkNo: " << std::to_string(chunkNo);
        LOG_ERROR("%s", errorReason.str().c_str());

        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        context.setResponseBody(json11::Json::object({{json::reason, errorReason.str()}}));
    }

    return returnCode;
}

auto FilesystemEndpoint::runGet(Context &context) -> sys::ReturnCodes
{
    LOG_DEBUG("Handling GET");
    auto returnCode = sys::ReturnCodes::Failure;

    if (context.getBody()[parserFSM::json::fileName].is_string()) {
        returnCode = startGetFile(context);
    }
    else if (context.getBody()[parserFSM::json::filesystem::rxID].is_number()) {
        returnCode = getFileChunk(context);
    }

    MessageHandler::putToSendQueue(context.createSimpleResponse());

    return returnCode;
}

auto FilesystemEndpoint::runPost(Context &context) -> sys::ReturnCodes
{
    LOG_DEBUG("Handling POST");
    sys::ReturnCodes returnCode = sys::ReturnCodes::Failure;
    std::string cmd             = context.getBody()[parserFSM::json::filesystem::command].string_value();

    context.setResponseBody(
        json11::Json::object({{json::status, std::to_string(static_cast<int>(sys::ReturnCodes::Failure))}}));

    auto owner = static_cast<ServiceDesktop *>(ownerServicePtr);

    if (cmd == parserFSM::json::filesystem::commands::download) {
        fs::path filePath    = context.getBody()[parserFSM::json::fileName].string_value();
        fs::path tmpFilePath = purefs::dir::getUpdatesOSPath() / filePath;

        const uint32_t fileSize = context.getBody()[parserFSM::json::fileSize].int_value();
        const auto fileCrc32    = context.getBody()[parserFSM::json::fileCrc32].string_value();

        LOG_DEBUG("got owner, file %s", tmpFilePath.c_str());

        if (isWritable(tmpFilePath)) {
            LOG_INFO("download %" PRIu32 " bytes to: %s", fileSize, tmpFilePath.c_str());

            if (owner->desktopWorker->startDownload(tmpFilePath, fileSize, fileCrc32) == sys::ReturnCodes::Success) {
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
