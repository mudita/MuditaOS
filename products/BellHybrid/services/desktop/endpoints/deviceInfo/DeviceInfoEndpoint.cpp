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
#include <purefs/filesystem_paths.hpp>

#include <ctime>

namespace sdesktop::endpoints
{
    auto DeviceInfoEndpoint::getSerialNumber() -> std::string
    {
        return static_cast<ServiceDesktop *>(ownerServicePtr)->getSerialNumber();
    }

    auto DeviceInfoEndpoint::getCaseColour() -> std::string
    {
        return static_cast<ServiceDesktop *>(ownerServicePtr)->getCaseColour();
    }

    auto DeviceInfoEndpoint::getOnboardingState() -> OnboardingState
    {
        return static_cast<ServiceDesktop *>(ownerServicePtr)->getOnboardingState();
    }

    auto DeviceInfoEndpoint::getDeviceInfo(Context &context) -> http::Code
    {
        const auto [totalDeviceSpaceMiB, reservedSystemSpaceMiB, usedUserSpaceMiB] = getStorageInfo();

        context.setResponseBody(json11::Json::object(
            {{json::batteryLevel, std::to_string(Store::Battery::get().level)},
             {json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
             {json::deviceSpaceTotal, std::to_string(totalDeviceSpaceMiB)},
             {json::systemReservedSpace, std::to_string(reservedSystemSpaceMiB)},
             {json::usedUserSpace, std::to_string(usedUserSpaceMiB)},
             {json::gitRevision, std::string(GIT_REV)},
             {json::gitBranch, std::string(GIT_BRANCH)},
             {json::currentRTCTime, std::to_string(static_cast<std::uint32_t>(std::time(nullptr)))},
             {json::version, std::string(VERSION)},
             {json::serialNumber, getSerialNumber()},
             {json::caseColour, getCaseColour()},
             {json::recoveryStatusFilePath,
              (purefs::dir::getTemporaryPath() / sdesktop::paths::recoveryStatusFilename).string()},
             {json::updateFilePath, (purefs::dir::getTemporaryPath() / sdesktop::paths::updateFilename).string()},
             {json::backupFilePath, (purefs::dir::getTemporaryPath() / sdesktop::paths::backupFilename).string()},
             {json::syncFilePath, (purefs::dir::getTemporaryPath() / sdesktop::paths::syncFilename).string()},
             {json::mtpPath, getMtpPath().string()},
             {json::onboardingState, std::to_string(static_cast<int>(getOnboardingState()))}}));

        return http::Code::OK;
    }

} // namespace sdesktop::endpoints
