// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/deviceInfo/DeviceInfoEndpointCommon.hpp>
#include <endpoints/message/Sender.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <sys/statvfs.h>

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
        if (not requestBody.object_items().empty() and requestBody[json::fileList].is_number()) {

            const auto diagFileType = parseDiagnosticFileType(requestBody[json::fileList]);

            if (!magic_enum::enum_contains<DiagnosticFileType>(diagFileType)) {
                LOG_ERROR("Bad diagnostic type %d requested", static_cast<unsigned>(diagFileType));
                return http::Code::BadRequest;
            }

            return gatherListOfDiagnostics(context, diagFileType);
        }

        return getDeviceInfo(context);
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
        case DiagnosticFileType::Logs:
            fileList = listDirectory(purefs::dir::getLogsPath());
            break;
        case DiagnosticFileType::CrashDumps:
            fileList = listDirectory(purefs::dir::getCrashDumpsPath());
            break;
        }

        if (!fileList.empty()) {
            status = http::Code::OK;
            context.setResponseBody(fileListToJsonObject(fileList));
        }

        return status;
    }

    auto DeviceInfoEndpointCommon::requestLogsFlush() const -> void
    {
        if (const auto owner = dynamic_cast<ServiceDesktop *>(ownerServicePtr); owner != nullptr) {
            owner->requestLogsFlush();
        }
    }

    auto DeviceInfoEndpointCommon::getMtpPath() const -> std::filesystem::path
    {
        if (const auto owner = dynamic_cast<ServiceDesktop *>(ownerServicePtr); owner != nullptr) {
            return owner->getMtpPath();
        }
        return std::filesystem::path{};
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
        constexpr auto bytesInMebibyte = 1024LLU * 1024LLU;
        struct statvfs vfstat
        {};

        if (statvfs(path.c_str(), &vfstat) < 0) {
            return {-1, -1};
        }

        const auto totalMbytes = static_cast<long>(
            (static_cast<std::uint64_t>(vfstat.f_blocks) * static_cast<std::uint64_t>(vfstat.f_bsize)) /
            bytesInMebibyte);
        const auto freeMbytes = static_cast<long>(
            (static_cast<std::uint64_t>(vfstat.f_bfree) * static_cast<std::uint64_t>(vfstat.f_bsize)) /
            bytesInMebibyte);

        return {totalMbytes, freeMbytes};
    }

    auto DeviceInfoEndpointCommon::getStorageInfo() -> std::tuple<long, long, long>
    {
        /* MuditaOS consists of two system partitions: 'system_a' and 'system_b'.
         * However, only one of them is mounted at the time. The value returned
         * by the endpoint should take into account space of both of them. */
        constexpr auto numberOfSystemPartitions = 2;

        long totalDeviceSpaceMiB    = 0;
        long reservedSystemSpaceMiB = 0;
        long usedUserSpaceMiB       = 0;

        /* System partitions stats */
        const auto systemDiskPath                      = purefs::dir::getSystemDiskPath();
        const auto [totalSystemSpace, freeSystemSpace] = getStorageStats(systemDiskPath);

        if ((totalSystemSpace < 0) || (freeSystemSpace < 0)) {
            LOG_ERROR("Failed to get stats for '%s'", systemDiskPath.c_str());
        }
        else {
            totalDeviceSpaceMiB = reservedSystemSpaceMiB = numberOfSystemPartitions * totalSystemSpace;
        }

        /* User partition stats */
        const auto userDiskPath                    = purefs::dir::getUserDiskPath();
        const auto [totalUserSpace, freeUserSpace] = getStorageStats(userDiskPath);

        if (totalUserSpace < 0 || freeUserSpace < 0) {
            LOG_ERROR("Failed to get stats for '%s'", userDiskPath.c_str());
        }
        else {
            usedUserSpaceMiB = totalUserSpace - freeUserSpace;
            totalDeviceSpaceMiB += totalUserSpace;
        }

        return {totalDeviceSpaceMiB, reservedSystemSpaceMiB, usedUserSpaceMiB};
    }
} // namespace sdesktop::endpoints
