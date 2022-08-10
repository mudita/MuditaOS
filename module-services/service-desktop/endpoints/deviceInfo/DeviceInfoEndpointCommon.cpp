// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/deviceInfo/DeviceInfoEndpointCommon.hpp>
#include <endpoints/message/Sender.hpp>
#include <endpoints/JsonKeyNames.hpp>

#include <EventStore.hpp>
#include <product/version.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <sys/statvfs.h>
#include <purefs/filesystem_paths.hpp>

#include <ctime>

namespace sdesktop::endpoints
{
    auto DeviceInfoEndpointCommon::handle(Context &context) -> void
    {
        http::Code status;
        switch (context.getMethod()) {
        case http::Method::get:
            status = handleGet(context);
            break;
        default:
            status = http::Code::BadRequest;
            break;
        }
        context.setResponseStatus(status);
        sender::putToSendQueue(context.createSimpleResponse());
    }

    auto DeviceInfoEndpointCommon::handleGet(Context &context) -> http::Code
    {
        const auto &requestBody = context.getBody();

        if (requestBody[json::fileList].is_number()) {

            const auto diagFileType = parseDiagnosticFileType(requestBody[json::fileList]);

            if (!magic_enum::enum_contains<DiagnosticFileType>(diagFileType)) {
                LOG_ERROR("Bad diagnostic type %d requested", static_cast<unsigned>(diagFileType));

                return http::Code::BadRequest;
            }

            return gatherListOfDiagnostics(context, diagFileType);
        }
        else {
            return getDeviceInfo(context);
        }
    }

    auto DeviceInfoEndpointCommon::parseDiagnosticFileType(const json11::Json &fileList) -> DiagnosticFileType
    {
        return magic_enum::enum_cast<DiagnosticFileType>(fileList.int_value()).value();
    }

    auto DeviceInfoEndpointCommon::gatherListOfDiagnostics(Context &context, DiagnosticFileType diagDataType)
        -> http::Code
    {
        std::vector<std::string> fileList;
        auto status = http::Code::NoContent;

        try {
            requestLogsFlush();
        }
        catch (const std::runtime_error &e) {
            LOG_ERROR("Logs flush exception: %s", e.what());
        }

        switch (diagDataType) {
        case DiagnosticFileType::Logs: {
            fileList = listDirectory(purefs::dir::getLogsPath());
            break;
        }

        case DiagnosticFileType::CrashDumps: {
            fileList = listDirectory(purefs::dir::getCrashDumpsPath());
            break;
        }
        }

        if (!fileList.empty()) {
            status = http::Code::OK;
            context.setResponseBody(fileListToJsonObject(fileList));
        }

        return status;
    }

    auto DeviceInfoEndpointCommon::requestLogsFlush() const -> void
    {
        auto owner = dynamic_cast<ServiceDesktop *>(ownerServicePtr);
        if (owner) {
            owner->requestLogsFlush();
        }
    }

    auto DeviceInfoEndpointCommon::fileListToJsonObject(const std::vector<std::string> &fileList) const
        -> json11::Json::object const
    {
        json11::Json::array fileArray;

        for (const auto &file : fileList) {
            fileArray.push_back(file);
        }

        return json11::Json::object{{json::files, fileArray}};
    }

    auto DeviceInfoEndpointCommon::listDirectory(const std::string &path) -> std::vector<std::string>
    {
        std::vector<std::string> entries;

        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            entries.push_back(entry.path());
        }

        return entries;
    }

    auto DeviceInfoEndpointCommon::getStorageStats(const std::string &path) -> std::tuple<long, long>
    {
        std::unique_ptr<struct statvfs> vfstat = std::make_unique<struct statvfs>();
        if (statvfs(path.c_str(), vfstat.get()) < 0) {
            return {-1, -1};
        }

        unsigned long totalMbytes = (uint64_t(vfstat->f_blocks) * uint64_t(vfstat->f_bsize)) / (1024LLU * 1024LLU);
        unsigned long freeMbytes  = (uint64_t(vfstat->f_bfree) * uint64_t(vfstat->f_bsize)) / (1024LLU * 1024LLU);

        return {totalMbytes, freeMbytes};
    }

    auto DeviceInfoEndpointCommon::getStorageInfo() -> std::tuple<long, long, long>
    {
        unsigned long totalDeviceSpaceMiB    = 0;
        unsigned long reservedSystemSpaceMiB = 0;
        unsigned long usedUserSpaceMiB       = 0;

        const std::array<std::filesystem::path, 2> systemStoragePaths{purefs::dir::getRootDiskPath(),
                                                                      purefs::dir::getPreviousOSPath()};
        for (const auto &p : systemStoragePaths) {
            auto [totalSpace, freeSpace] = getStorageStats(p);

            if (totalSpace < 0 || freeSpace < 0) {
                LOG_ERROR("Failed to get stats for %s", p.c_str());
                continue;
            }

            totalDeviceSpaceMiB += totalSpace;
            reservedSystemSpaceMiB = totalDeviceSpaceMiB;
        }

        // User partition stats
        const auto userStoragePath   = purefs::dir::getUserDiskPath();
        auto [totalSpace, freeSpace] = getStorageStats(userStoragePath);

        if (totalSpace < 0 || freeSpace < 0) {
            LOG_ERROR("Failed to get stats for %s", userStoragePath.c_str());
        }
        else {
            usedUserSpaceMiB = totalSpace - freeSpace;
            totalDeviceSpaceMiB += totalSpace;
        }

        return {totalDeviceSpaceMiB, reservedSystemSpaceMiB, usedUserSpaceMiB};
    }
} // namespace sdesktop::endpoints
