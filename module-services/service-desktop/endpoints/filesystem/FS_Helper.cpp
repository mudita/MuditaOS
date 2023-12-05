// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/filesystem/FS_Helper.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <endpoints/message/Sender.hpp>

#include <sys/statvfs.h>

namespace sdesktop::endpoints
{
    namespace
    {
        constexpr auto bytesInMebibyte = 1024LLU * 1024LLU;

        enum FileType
        {
            directory,
            regularFile,
            symlink,
            other,
        };

        auto parseFileEntry(const std::filesystem::directory_entry &entry) -> json11::Json
        {
            uintmax_t size = 0;
            FileType type;

            if (entry.is_directory()) {
                type = FileType::directory;
            }
            else {
                size = entry.file_size();
            }

            if (entry.is_regular_file()) {
                type = FileType::regularFile;
            }
            else if (entry.is_symlink()) {
                type = FileType::symlink;
            }
            else {
                type = FileType::other;
            }

            return json11::Json::object{
                {json::fs::path, entry.path().string()}, {json::fs::fileSize, size}, {json::fs::type, type}};
        }
    } // namespace

    auto FS_Helper::processGet(Context &context) -> ProcessResult
    {
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
            LOG_ERROR("Unknown request");
            response = {.status = http::Code::BadRequest};
        }

        return {sent::no, std::move(response)};
    }

    auto FS_Helper::processPut(Context &context) -> ProcessResult
    {
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
        const auto &body = context.getBody();
        auto code        = http::Code::BadRequest;

        if (body[json::fs::removeFile].is_string()) {
            const auto &fileName = body[json::fs::removeFile].string_value();
            try {
                code = requestFileRemoval(fileName) ? http::Code::NoContent : http::Code::NotFound;
            }
            catch (const std::filesystem::filesystem_error &ex) {
                LOG_ERROR("Can't remove requested file %s, error: %d", fileName.c_str(), ex.code().value());
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
        if (ownerService != nullptr) {
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
            LOG_ERROR("File %s not found", filePath.c_str());

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
            LOG_ERROR("Exception during getting data: %s", e.what());

            json11::Json::object response({{json::reason, e.what()}});
            return ResponseContext{.status = http::Code::BadRequest, .body = response};
        }

        json11::Json::object response{};
        auto code = http::Code::BadRequest;

        if (!dataWithCrc32.data.empty()) {
            code     = http::Code::OK;
            response = json11::Json::object({{json::fs::rxID, static_cast<int>(rxID)},
                                             {json::fs::chunkNo, static_cast<int>(chunkNo)},
                                             {json::fs::data, dataWithCrc32.data}});

            if (!dataWithCrc32.crc32.empty()) {
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
        const auto &body             = context.getBody();
        const auto &filePath         = body[json::fs::fileName].string_value();
        const std::uint32_t fileSize = body[json::fs::fileSize].int_value();
        const auto &fileCrc32        = body[json::fs::fileCrc32].string_value();
        auto code                    = http::Code::BadRequest;

        LOG_DEBUG("Start sending of file: %s", filePath.c_str());

        if (fileSize == 0 || fileCrc32.empty()) {
            LOG_ERROR("File '%s' corrupted", filePath.c_str());

            return ResponseContext{.status = code};
        }

        const auto freeSpaceLeftForUserFilesMiB = getFreeSpaceForUserFilesMiB();

        if ((freeSpaceLeftForUserFilesMiB - (static_cast<float>(fileSize) / bytesInMebibyte)) <= 0) {
            LOG_ERROR("Not enough space left on device!");
            code = http::Code::InsufficientStorage;
            return ResponseContext{.status = code};
        }

        if (!std::filesystem::exists(filePath)) {
            LOG_DEBUG("Creating file %s", filePath.c_str());
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
        const auto &data   = body[json::fs::data].string_value();

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
            LOG_ERROR("Exception during sending data: %s", e.what());
            auto code     = http::Code::NotAcceptable;
            auto response = json11::Json::object({{json::reason, e.what()}});

            return ResponseContext{.status = code, .body = response};
        }

        json11::Json::object response{};
        auto code = http::Code::OK;

        if (returnCode == sys::ReturnCodes::Success) {
            response = json11::Json::object(
                {{json::fs::txID, static_cast<int>(txID)}, {json::fs::chunkNo, static_cast<int>(chunkNo)}});
        }
        else {
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

    auto FS_Helper::requestListDir(const std::string &directory) -> ResponseContext
    {
        if (!std::filesystem::exists(directory)) {
            return ResponseContext{.status = http::Code::NotFound};
        }

        json11::Json::array jsonArr;

        for (const auto &entry : std::filesystem::directory_iterator{directory}) {
            jsonArr.push_back(parseFileEntry(entry));
        }

        json11::Json::object const response({{directory, jsonArr}});
        return ResponseContext{.status = http::Code::OK, .body = response};
    }

    auto FS_Helper::getFreeSpaceForUserFilesMiB() const -> float
    {
        const auto userDiskPath = purefs::dir::getUserDiskPath();
        struct statvfs vfstat
        {};

        if (statvfs(userDiskPath.c_str(), &vfstat) < 0) {
            return 0;
        }

        const auto freeBytes = (static_cast<float>(vfstat.f_bfree) * static_cast<float>(vfstat.f_bsize));
        const auto freeMiBs  = freeBytes / bytesInMebibyte;

        return freeMiBs;
    }
} // namespace sdesktop::endpoints
