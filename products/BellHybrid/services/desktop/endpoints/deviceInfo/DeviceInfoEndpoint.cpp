// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/deviceInfo/DeviceInfoEndpoint.hpp>
#include <endpoints/message/Sender.hpp>
#include <endpoints/JsonKeyNames.hpp>

#include <EventStore.hpp>
#include <product/version.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <cstdint>
#include <string>
#include <sys/statvfs.h>
#include <purefs/filesystem_paths.hpp>

#include <ctime>

namespace sdesktop::endpoints
{

    auto DeviceInfoEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::get:
            getDeviceInfo(context);
            break;
        default:
            context.setResponseStatus(http::Code::BadRequest);
            sender::putToSendQueue(context.createSimpleResponse());
            break;
        }
    }

    auto DeviceInfoEndpoint::getSerialNumber() -> std::string
    {
        return dynamic_cast<ServiceDesktop *>(ownerServicePtr)->getSerialNumber();
    }

    auto DeviceInfoEndpoint::getStorageStats(const std::string &path) -> std::tuple<long, long>
    {
        std::unique_ptr<struct statvfs> vfstat = std::make_unique<struct statvfs>();
        if (statvfs(path.c_str(), vfstat.get()) < 0) {
            return {-1, -1};
        }

        unsigned long totalMbytes = (uint64_t(vfstat->f_blocks) * uint64_t(vfstat->f_bsize)) / (1024LLU * 1024LLU);
        unsigned long freeMbytes  = (uint64_t(vfstat->f_bfree) * uint64_t(vfstat->f_bsize)) / (1024LLU * 1024LLU);

        return {totalMbytes, freeMbytes};
    }

    auto DeviceInfoEndpoint::getDeviceInfo(Context &context) -> bool
    {
        if (ownerServicePtr == nullptr) {
            context.setResponseStatus(http::Code::InternalServerError);
            return false;
        }

        const std::array<std::filesystem::path, 2> storagePaths{purefs::dir::getRootDiskPath(),
                                                                purefs::dir::getPreviousOSPath()};

        unsigned long totalMbytes    = 0;
        unsigned long freeUserMbytes = 0;
        unsigned long freePercent    = 0;

        for (const auto &p : storagePaths) {
            auto [totalSpace, freeSpace] = getStorageStats(p);

            if (totalSpace < 0 || freeSpace < 0) {
                LOG_ERROR("Failed to get stats for %s", p.c_str());
                continue;
            }

            totalMbytes += totalSpace;
        }

        // User partition stats
        const auto storagePath       = purefs::dir::getUserDiskPath();
        auto [totalSpace, freeSpace] = getStorageStats(storagePath);

        if (totalSpace < 0 || freeSpace < 0) {
            LOG_ERROR("Failed to get stats for %s", storagePath.c_str());
        }
        else {
            totalMbytes += totalSpace;
            freeUserMbytes = freeSpace;

            // Deduct 1024 MB reserved for OS data on User partition
            freeUserMbytes -= OS_RESERVED_SPACE_IN_MB;
        }

        if (totalMbytes) {
            freePercent = (freeUserMbytes * 100) / totalMbytes;
        }

        context.setResponseBody(
            json11::Json::object({{json::batteryLevel, std::to_string(Store::Battery::get().level)},
                                  {json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
                                  {json::fsTotal, std::to_string(totalMbytes)},
                                  {json::fsFree, std::to_string(freeUserMbytes)},
                                  {json::fsFreePercent, std::to_string(freePercent)},
                                  {json::gitRevision, (std::string)(GIT_REV)},
                                  {json::gitBranch, (std::string)GIT_BRANCH},
                                  {json::currentRTCTime, std::to_string(static_cast<uint32_t>(std::time(nullptr)))},
                                  {json::version, std::string(VERSION)},
                                  {json::serialNumber, getSerialNumber()}}));

        sender::putToSendQueue(context.createSimpleResponse());
        return true;
    }

} // namespace sdesktop::endpoints
