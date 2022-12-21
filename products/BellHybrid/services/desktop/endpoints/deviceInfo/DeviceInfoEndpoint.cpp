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
#include <vector>
#include <sys/statvfs.h>
#include <purefs/filesystem_paths.hpp>
#include <purefs/vfs_subsystem.hpp>

#include <ctime>
namespace
{
    constexpr auto DEVICE_NAME      = "emmc0sys1";
    constexpr auto EMMC_SN_LENGTH   = 13;
    constexpr auto EMMC_BUFFER_SIZE = 512;
    constexpr auto SECTOR_TO_READ   = 2;
    constexpr auto SECTOR_COUNT     = 1U;

    std::string readSerialNumberFromEmmc()
    {
        char buffer[EMMC_BUFFER_SIZE];
        purefs::subsystem::disk_mgr()->read(DEVICE_NAME, buffer, SECTOR_TO_READ, SECTOR_COUNT);
        buffer[EMMC_SN_LENGTH] = '\0';
        return std::string(buffer);
    }

} // namespace
namespace sdesktop::endpoints
{

    auto DeviceInfoEndpoint::getSerialNumber() -> std::string
    {
        return readSerialNumberFromEmmc();
    }

    auto DeviceInfoEndpoint::getDeviceInfo(Context &context) -> http::Code
    {
        auto [totalDeviceSpaceMiB, reservedSystemSpaceMiB, usedUserSpaceMiB] = getStorageInfo();

        context.setResponseBody(json11::Json::object(
            {{json::batteryLevel, std::to_string(Store::Battery::get().level)},
             {json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
             {json::deviceSpaceTotal, std::to_string(totalDeviceSpaceMiB)},
             {json::systemReservedSpace, std::to_string(reservedSystemSpaceMiB)},
             {json::usedUserSpace, std::to_string(usedUserSpaceMiB)},
             {json::gitRevision, (std::string)(GIT_REV)},
             {json::gitBranch, (std::string)GIT_BRANCH},
             {json::currentRTCTime, std::to_string(static_cast<uint32_t>(std::time(nullptr)))},
             {json::version, std::string(VERSION)},
             {json::serialNumber, getSerialNumber()},
             {json::recoveryStatusFilePath,
              (purefs::dir::getTemporaryPath() / sdesktop::paths::recoveryStatusFilename).string()},
             {json::updateFilePath, (purefs::dir::getTemporaryPath() / sdesktop::paths::updateFilename).string()},
             {json::backupFilePath, (purefs::dir::getTemporaryPath() / sdesktop::paths::backupFilename).string()},
             {json::syncFilePath, (purefs::dir::getTemporaryPath() / sdesktop::paths::syncFilename).string()},
             {json::mtpPath, purefs::dir::getRelaxationPath().string()}}));

        return http::Code::OK;
    }

} // namespace sdesktop::endpoints
