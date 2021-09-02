// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FilesystemEndpoint.hpp"
#include "FileOperations.hpp"
#include "service-desktop/DesktopMessages.hpp"
#include "service-desktop/ServiceDesktop.hpp"
#include <purefs/filesystem_paths.hpp>

#include <filesystem>

using namespace parserFSM;
namespace fs = std::filesystem;

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
    case http::Method::put:
        returnCode = runPut(context);
        break;
    default:
        LOG_ERROR("Unhandled method request: %u", static_cast<unsigned>(context.getMethod()));
        returnCode = sys::ReturnCodes::Failure;
        break;
    }
    LOG_DEBUG("returnCode: %u", static_cast<unsigned>(returnCode));
}

auto FilesystemEndpoint::requestLogsFlush() const -> void
{
    auto owner = dynamic_cast<ServiceDesktop *>(ownerServicePtr);
    if (owner) {
        owner->requestLogsFlush();
    }
}

auto FilesystemEndpoint::startGetFile(Context &context) const -> sys::ReturnCodes
{
    std::filesystem::path filePath = context.getBody()[parserFSM::json::fileName].string_value();

    try {
        requestLogsFlush();
    }
    catch (const std::runtime_error &e) {
        LOG_ERROR("Logs flush exception: %s", e.what());

        context.setResponseStatus(parserFSM::http::Code::InternalServerError);
        context.setResponseBody(json11::Json::object({{json::reason, std::string(e.what())}}));
        return sys::ReturnCodes::Failure;
    }

    if (!std::filesystem::exists(filePath)) {
        LOG_ERROR("file not found");

        context.setResponseStatus(parserFSM::http::Code::NotFound);
        context.setResponseBody(json11::Json::object({{json::reason, json::filesystem::reasons::fileDoesNotExist}}));
        return sys::ReturnCodes::Failure;
    }

    LOG_DEBUG("Checking file");

    try {
        auto [rxID, fileSize] = fileOps.createReceiveIDForFile(filePath);

        auto fileHash = fileOps.getFileHashForReceiveID(rxID);

        context.setResponseStatus(parserFSM::http::Code::OK);
        context.setResponseBody(
            json11::Json::object({{json::filesystem::rxID, static_cast<int>(rxID)},
                                  {json::fileSize, static_cast<int>(fileSize)},
                                  {json::fileCrc32, fileHash},
                                  {json::filesystem::chunkSize, static_cast<int>(FileOperations::ChunkSize)}}));
    }
    catch (std::runtime_error &e) {
        LOG_ERROR("FileOperations exception: %s", e.what());

        context.setResponseStatus(parserFSM::http::Code::InternalServerError);
        context.setResponseBody(json11::Json::object({{json::reason, std::string(e.what())}}));
        return sys::ReturnCodes::Failure;
    }
    catch (std::exception &e) {
        LOG_ERROR("FileOperations exception: %s", e.what());

        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        context.setResponseBody(json11::Json::object({{json::reason, std::string(e.what())}}));
        return sys::ReturnCodes::Failure;
    }

    return sys::ReturnCodes::Success;
}

auto FilesystemEndpoint::getFileChunk(Context &context) const -> sys::ReturnCodes
{
    auto returnCode    = sys::ReturnCodes::Failure;
    const auto rxID    = context.getBody()[parserFSM::json::filesystem::rxID].int_value();
    const auto chunkNo = context.getBody()[parserFSM::json::filesystem::chunkNo].int_value();
    std::string data{};

    try {
        data = fileOps.getDataForReceiveID(rxID, chunkNo);
    }
    catch (std::exception &e) {
        LOG_ERROR("%s", e.what());
        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        context.setResponseBody(json11::Json::object({{json::reason, e.what()}}));

        return sys::ReturnCodes::Failure;
    }

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
    else {
        LOG_ERROR("unknown request");
        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        returnCode = sys::ReturnCodes::Failure;
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

    if (cmd == parserFSM::json::filesystem::commands::checkFile) {
        fs::path filePath = context.getBody()[parserFSM::json::fileName].string_value();
        LOG_DEBUG("Checking file");

        context.setResponseBody(json11::Json::object{{json::fileExists, std::filesystem::exists(filePath)}});
        returnCode = sys::ReturnCodes::Success;
    }
    else {
        LOG_ERROR("unknown command");
        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        returnCode = sys::ReturnCodes::Failure;
    }

    MessageHandler::putToSendQueue(context.createSimpleResponse());
    return returnCode;
}

auto FilesystemEndpoint::runPut(Context &context) -> sys::ReturnCodes
{
    LOG_DEBUG("Handling PUT");
    auto returnCode         = sys::ReturnCodes::Failure;
    const auto &requestBody = context.getBody();

    if (requestBody[parserFSM::json::fileName].is_string() && requestBody[parserFSM::json::fileSize].is_number() &&
        requestBody[parserFSM::json::fileCrc32].is_string()) {
        returnCode = startSendFile(context);
    }
    else if (requestBody[parserFSM::json::filesystem::txID].is_number() &&
             requestBody[parserFSM::json::filesystem::chunkNo].is_number() &&
             requestBody[parserFSM::json::filesystem::data].is_string()) {
        returnCode = sendFileChunk(context);
    }
    else {
        LOG_ERROR("unknown request");
        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        returnCode = sys::ReturnCodes::Failure;
    }

    MessageHandler::putToSendQueue(context.createSimpleResponse());
    return returnCode;
}

auto FilesystemEndpoint::startSendFile(parserFSM::Context &context) const -> sys::ReturnCodes
{
    auto returnCode         = sys::ReturnCodes::Failure;
    const auto &requestBody = context.getBody();

    std::filesystem::path filePath = requestBody[parserFSM::json::fileName].string_value();
    const uint32_t fileSize        = requestBody[parserFSM::json::fileSize].int_value();
    const auto fileCrc32           = requestBody[parserFSM::json::fileCrc32].string_value();

    LOG_DEBUG("Start sending of file: %s", filePath.c_str());

    if (fileSize == 0 || fileCrc32.empty()) {
        LOG_ERROR("File %s corrupted", filePath.c_str());

        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        return returnCode;
    }

    if (!std::filesystem::exists(filePath)) {
        LOG_DEBUG("Creating file %s", filePath.c_str());

        context.setResponseStatus(parserFSM::http::Code::Created);
    }
    else {
        LOG_DEBUG("Overwriting file %s", filePath.c_str());
    }

    try {
        auto txID = fileOps.createTransmitIDForFile(filePath, fileSize, fileCrc32);

        context.setResponseStatus(parserFSM::http::Code::OK);
        context.setResponseBody(
            json11::Json::object({{json::filesystem::txID, static_cast<int>(txID)},
                                  {json::filesystem::chunkSize, static_cast<int>(FileOperations::ChunkSize)}}));
    }
    catch (std::runtime_error &e) {
        LOG_ERROR("FileOperations exception: %s", e.what());

        context.setResponseStatus(parserFSM::http::Code::InternalServerError);
        context.setResponseBody(json11::Json::object({{json::reason, std::string(e.what())}}));
        return sys::ReturnCodes::Failure;
    }
    catch (std::exception &e) {
        LOG_ERROR("FileOperations exception: %s", e.what());

        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        context.setResponseBody(json11::Json::object({{json::reason, std::string(e.what())}}));
        return sys::ReturnCodes::Failure;
    }

    return sys::ReturnCodes::Success;
}

auto FilesystemEndpoint::sendFileChunk(parserFSM::Context &context) const -> sys::ReturnCodes
{
    auto returnCode         = sys::ReturnCodes::Failure;
    const auto &requestBody = context.getBody();
    const auto txID         = requestBody[parserFSM::json::filesystem::txID].int_value();
    const auto chunkNo      = requestBody[parserFSM::json::filesystem::chunkNo].int_value();
    const auto data         = requestBody[parserFSM::json::filesystem::data].string_value();

    if (data.empty()) {
        std::ostringstream errorReason;
        errorReason << "Invalid request txID: " << std::to_string(txID) << ", chunkNo: " << std::to_string(chunkNo);
        LOG_ERROR("%s", errorReason.str().c_str());

        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        context.setResponseBody(json11::Json::object({{json::reason, errorReason.str()}}));
        return returnCode;
    }

    try {
        returnCode = fileOps.sendDataForTransmitID(txID, chunkNo, data);
    }
    catch (std::exception &e) {
        LOG_ERROR("%s", e.what());
        context.setResponseStatus(parserFSM::http::Code::NotAcceptable);
        context.setResponseBody(json11::Json::object({{json::reason, e.what()}}));

        return sys::ReturnCodes::Failure;
    }

    if (returnCode == sys::ReturnCodes::Success) {
        LOG_DEBUG("FileOperations::sendDataForTransmitID success");
        context.setResponseStatus(parserFSM::http::Code::OK);
        context.setResponseBody(json11::Json::object({
            {json::filesystem::txID, static_cast<int>(txID)},
            {json::filesystem::chunkNo, static_cast<int>(chunkNo)},
        }));
    }
    else {
        LOG_ERROR("FileOperations::sendDataForTransmitID failed");
        context.setResponseStatus(parserFSM::http::Code::BadRequest);
        context.setResponseBody(json11::Json::object({
            {json::filesystem::txID, static_cast<int>(txID)},
            {json::filesystem::chunkNo, static_cast<int>(chunkNo)},
        }));
    }

    return returnCode;
}
