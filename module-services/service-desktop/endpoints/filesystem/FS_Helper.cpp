// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/filesystem/FS_Helper.hpp>
#include <endpoints/Context.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <endpoints/message/Sender.hpp>
#include <purefs/filesystem_paths.hpp>

#include <sys/statvfs.h>
#include <filesystem>

namespace sdesktop::endpoints
{
    namespace
    {
        constexpr auto bytesInMebibyte = 1024LLU * 1024LLU;
    }

    using sender::putToSendQueue;
    namespace fs = std::filesystem;

    auto FS_Helper::processGet(Context &context) -> ProcessResult
    {
        LOG_DEBUG("Handling GET");
        const auto &body = context.getBody();
        ResponseContext response{};

        if (body[json::fs::fileName].is_string()) {
            response = startGetFile(context);
        }
        else if (body[json::fs::rxID].is_number()) {
            response = getFileChunk(context);
        }
        else if (body[json::fs::listDir].is_string()) {
            const auto &dir = body[json::fs::listDir].string_value();
            response        = requestListDir(dir);
        }
        else {
            LOG_ERROR("unknown request");
            response = {.status = http::Code::BadRequest};
        }

        return {sent::no, std::move(response)};
    }

    auto FS_Helper::processPut(Context &context) -> ProcessResult
    {
        LOG_DEBUG("Handling PUT");
        const auto &body = context.getBody();
        auto code        = http::Code::BadRequest;
        ResponseContext response{};

        if (body[json::fs::fileName].is_string() && body[json::fs::fileSize].is_number() &&
            body[json::fs::fileCrc32].is_string()) {
            response = startSendFile(context);
        }
        else if (body[json::fs::txID].is_number() && body[json::fs::chunkNo].is_number() &&
                 body[json::fs::data].is_string()) {
            response = sendFileChunk(context);
        }
        else if (body[json::fs::renameFile].is_string() && body[json::fs::destFilename].is_string()) {
            const auto &fileName     = body[json::fs::renameFile].string_value();
            const auto &destFilename = body[json::fs::destFilename].string_value();
            code = requestFileRename(fileName, destFilename) ? http::Code::NoContent : http::Code::NotFound;

            response = ResponseContext{.status = code};
        }
        else {
            LOG_ERROR("Bad request, missing or invalid argument");
            response = ResponseContext{.status = http::Code::BadRequest};
        }

        return {sent::no, std::move(response)};
    }

    auto FS_Helper::processDelete(Context &context) -> ProcessResult
    {
        LOG_DEBUG("Handling DEL");
        const auto &body = context.getBody();
        auto code        = http::Code::BadRequest;

        if (body[json::fs::removeFile].is_string()) {
            const auto &fileName = body[json::fs::removeFile].string_value();
            try {
                code = requestFileRemoval(fileName) ? http::Code::NoContent : http::Code::NotFound;
            }
            catch (const std::filesystem::filesystem_error &ex) {
                LOG_ERROR("Can't remove requested file, error: %d", ex.code().value());
                code = http::Code::InternalServerError;
            }
        }
        else {
            LOG_ERROR("Bad request, missing or invalid argument");
        }

        return {sent::no, ResponseContext{.status = code}};
    }
    auto FS_Helper::requestLogsFlush() const -> void
    {
        auto ownerService = dynamic_cast<ServiceDesktop *>(owner);
        if (ownerService) {
            ownerService->requestLogsFlush();
        }
    }

    auto FS_Helper::startGetFile(Context &context) const -> ResponseContext
    {
        const std::filesystem::path filePath = context.getBody()[json::fs::fileName].string_value();

        try {
            requestLogsFlush();
        }
        catch (const std::runtime_error &e) {
            LOG_ERROR("Logs flush exception: %s", e.what());

            json11::Json::object response({{json::reason, e.what()}});
            return ResponseContext{.status = http::Code::InternalServerError, .body = response};
        }

        if (!std::filesystem::exists(filePath)) {
            LOG_ERROR("file not found");

            json11::Json::object response({{json::reason, json::fs::fileDoesNotExist}});
            return ResponseContext{.status = http::Code::NotFound, .body = response};
        }

        json11::Json::object response{};
        auto code = http::Code::BadRequest;

        LOG_DEBUG("Checking file");

        try {
            auto [rxID, fileSize] = fileOps.createReceiveIDForFile(filePath);

            code     = http::Code::OK;
            response = json11::Json::object({{json::fs::rxID, static_cast<int>(rxID)},
                                             {json::fs::fileSize, static_cast<int>(fileSize)},
                                             {json::fs::chunkSize, static_cast<int>(FileOperations::ChunkSize)}});
        }
        catch (std::runtime_error &e) {
            LOG_ERROR("FileOperations exception: %s", e.what());

            code     = http::Code::InternalServerError;
            response = json11::Json::object({{json::reason, std::string(e.what())}});
        }
        catch (std::exception &e) {
            LOG_ERROR("FileOperations exception: %s", e.what());

            code     = http::Code::BadRequest;
            response = json11::Json::object({{json::reason, std::string(e.what())}});
        }

        return ResponseContext{.status = code, .body = response};
    }

    auto FS_Helper::getFileChunk(Context &context) const -> ResponseContext
    {
        const auto rxID    = context.getBody()[json::fs::rxID].int_value();
        const auto chunkNo = context.getBody()[json::fs::chunkNo].int_value();
        FileOperations::DataWithCrc32 dataWithCrc32;

        try {
            dataWithCrc32 = fileOps.getDataForReceiveID(rxID, chunkNo);
        }
        catch (std::exception &e) {
            LOG_ERROR("%s", e.what());

            json11::Json::object response({{json::reason, e.what()}});
            return ResponseContext{.status = http::Code::BadRequest, .body = response};
        }

        json11::Json::object response{};
        auto code = http::Code::BadRequest;

        if (dataWithCrc32.data.length()) {
            code     = http::Code::OK;
            response = json11::Json::object({{json::fs::rxID, static_cast<int>(rxID)},
                                             {json::fs::chunkNo, static_cast<int>(chunkNo)},
                                             {json::fs::data, dataWithCrc32.data}});

            if (dataWithCrc32.crc32.length()) {
                response[json::fs::fileCrc32] = dataWithCrc32.crc32;
            }
        }
        else {
            std::ostringstream errorReason;
            errorReason << "Invalid request rxID: " << std::to_string(rxID) << ", chunkNo: " << std::to_string(chunkNo);
            LOG_ERROR("%s", errorReason.str().c_str());

            code     = http::Code::BadRequest;
            response = json11::Json::object({{json::reason, errorReason.str()}});
        }

        return ResponseContext{.status = code, .body = response};
    }

    auto FS_Helper::startSendFile(Context &context) const -> ResponseContext
    {
        const auto &body               = context.getBody();
        std::filesystem::path filePath = body[json::fs::fileName].string_value();
        const uint32_t fileSize        = body[json::fs::fileSize].int_value();
        const auto fileCrc32           = body[json::fs::fileCrc32].string_value();
        auto code                      = http::Code::BadRequest;

        LOG_DEBUG("Start sending of file: %s", filePath.c_str());

        if (fileSize == 0 || fileCrc32.empty()) {
            LOG_ERROR("File %s corrupted", filePath.c_str());

            return ResponseContext{.status = code};
        }

        const auto freeSpaceLeftForUserFilesMiB = getFreeSpaceForUserFilesMiB();

        if (fileSize / bytesInMebibyte > freeSpaceLeftForUserFilesMiB) {
            LOG_ERROR("Not enough space left on device!");
            code = http::Code::InsufficientStorage;
            return ResponseContext{.status = code};
        }

        if (!std::filesystem::exists(filePath)) {
            LOG_DEBUG("Creating file %s", filePath.c_str());

            code = http::Code::Created;
        }
        else {
            LOG_DEBUG("Overwriting file %s", filePath.c_str());
        }

        json11::Json::object response{};

        try {
            auto txID = fileOps.createTransmitIDForFile(filePath, fileSize, fileCrc32);

            code     = http::Code::OK;
            response = json11::Json::object({{json::fs::txID, static_cast<int>(txID)},
                                             {json::fs::chunkSize, static_cast<int>(FileOperations::ChunkSize)}});
        }
        catch (std::runtime_error &e) {
            LOG_ERROR("FileOperations exception: %s", e.what());

            code     = http::Code::InternalServerError;
            response = json11::Json::object({{json::reason, std::string(e.what())}});
        }
        catch (std::exception &e) {
            LOG_ERROR("FileOperations exception: %s", e.what());

            code     = http::Code::BadRequest;
            response = json11::Json::object({{json::reason, std::string(e.what())}});
        }

        return ResponseContext{.status = code, .body = response};
    }

    auto FS_Helper::sendFileChunk(Context &context) const -> ResponseContext
    {
        const auto &body   = context.getBody();
        const auto txID    = body[json::fs::txID].int_value();
        const auto chunkNo = body[json::fs::chunkNo].int_value();
        const auto data    = body[json::fs::data].string_value();

        if (data.empty()) {
            std::ostringstream errorReason;
            errorReason << "Invalid request txID: " << std::to_string(txID) << ", chunkNo: " << std::to_string(chunkNo);
            LOG_ERROR("%s", errorReason.str().c_str());

            auto code     = http::Code::BadRequest;
            auto response = json11::Json::object({{json::reason, errorReason.str()}});

            return ResponseContext{.status = code, .body = response};
        }

        auto returnCode = sys::ReturnCodes::Success;

        try {
            returnCode = fileOps.sendDataForTransmitID(txID, chunkNo, data);
        }
        catch (std::exception &e) {
            LOG_ERROR("%s", e.what());

            auto code     = http::Code::NotAcceptable;
            auto response = json11::Json::object({{json::reason, e.what()}});

            return ResponseContext{.status = code, .body = response};
        }

        json11::Json::object response{};
        auto code = http::Code::OK;

        if (returnCode == sys::ReturnCodes::Success) {
            LOG_DEBUG("FileOperations::sendDataForTransmitID success");

            response = json11::Json::object(
                {{json::fs::txID, static_cast<int>(txID)}, {json::fs::chunkNo, static_cast<int>(chunkNo)}});
        }
        else {
            LOG_ERROR("FileOperations::sendDataForTransmitID failed");

            code     = http::Code::BadRequest;
            response = json11::Json::object(
                {{json::fs::txID, static_cast<int>(txID)}, {json::fs::chunkNo, static_cast<int>(chunkNo)}});
        }

        return ResponseContext{.status = code, .body = response};
    }

    auto FS_Helper::requestFileRemoval(const std::string &fileName) -> bool
    {
        return std::filesystem::remove(fileName);
    }

    auto FS_Helper::requestFileRename(const std::string &fileName, const std::string &destFilename) noexcept -> bool
    {
        std::error_code ec;
        std::filesystem::rename(fileName, destFilename, ec);
        if (!ec) {
            LOG_ERROR("Failed to rename %s, error: %d", fileName.c_str(), ec.value());
        }
        return !ec;
    }

    auto FS_Helper::parseFileEntry(const std::filesystem::directory_entry &entry) -> FileEntry
    {
        FileEntry fileEntry;
        fileEntry.filename = entry.path();

        if (entry.is_directory()) {
            fileEntry.type = FileType::directory;
            return fileEntry;
        }

        fileEntry.size = entry.file_size();
        if (entry.is_regular_file()) {
            fileEntry.type = FileType::regularFile;
        }
        else if (entry.is_symlink()) {
            fileEntry.type = FileType::symlink;
        }
        else {
            fileEntry.type = FileType::other;
        }

        return fileEntry;
    }

    auto FS_Helper::requestListDir(const std::string &directory) -> ResponseContext
    {
        using FileEntriesVector = std::vector<FileEntry>;

        if (!std::filesystem::exists(directory)) {
            return ResponseContext{.status = http::Code::NotFound};
        }

        FileEntriesVector filesInDir;
        for (const auto &entry : std::filesystem::directory_iterator{directory}) {
            filesInDir.emplace_back(parseFileEntry(entry));
        }

        json11::Json::array jsonArr;
        jsonArr.reserve(filesInDir.size());

        for (const auto &fileEntry : filesInDir) {
            jsonArr.push_back(json11::Json::object{{json::fs::path, fileEntry.filename},
                                                   {json::fs::fileSize, static_cast<int>(fileEntry.size)},
                                                   {json::fs::type, fileEntry.type}});
        }

        json11::Json::object const response({{directory, jsonArr}});
        return ResponseContext{.status = http::Code::OK, .body = response};
    }

    auto FS_Helper::getFreeSpaceForUserFilesMiB() const -> unsigned long
    {
        const auto userDiskPath = purefs::dir::getUserDiskPath();

        std::unique_ptr<struct statvfs> vfstat = std::make_unique<struct statvfs>();
        if (statvfs(userDiskPath.c_str(), vfstat.get()) < 0) {
            return 0;
        }

        const auto freeBytes =
            (static_cast<std::uint64_t>(vfstat->f_bfree) * static_cast<std::uint64_t>(vfstat->f_bsize));
        const auto freeMiBs = freeBytes / bytesInMebibyte;

        return freeMiBs;
    }
} // namespace sdesktop::endpoints
