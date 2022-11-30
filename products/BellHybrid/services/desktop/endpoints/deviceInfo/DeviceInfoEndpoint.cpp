// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
#include <serial-number-reader/SerialNumberReader.hpp>

#include <ctime>

namespace sdesktop::endpoints
{

    auto DeviceInfoEndpoint::getSerialNumber() -> std::string
    {
        return serial_number_reader::readSerialNumber();
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
             {json::recoveryStatusFilePath, purefs::dir::getTemporaryPath() / recoveryStatusFilename},
             {json::updateFilePath, purefs::dir::getTemporaryPath() / updateFilename},
             {json::backupFilePath, purefs::dir::getTemporaryPath() / backupFilename},
             {json::syncFilePath, purefs::dir::getTemporaryPath() / syncFilename}}));

        return http::Code::OK;
    }

} // namespace sdesktop::endpoints
