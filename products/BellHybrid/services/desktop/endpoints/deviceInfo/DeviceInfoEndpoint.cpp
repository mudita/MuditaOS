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

#include <ctime>

namespace sdesktop::endpoints
{

    auto DeviceInfoEndpoint::getDeviceInfo(Context &context) -> http::Code
    {
        auto [totalMbytes, freeUserMbytes, freePercent] = getStorageInfo();

        context.setResponseBody(
            json11::Json::object({{json::batteryLevel, std::to_string(Store::Battery::get().level)},
                                  {json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
                                  {json::fsTotal, std::to_string(totalMbytes)},
                                  {json::fsFree, std::to_string(freeUserMbytes)},
                                  {json::fsFreePercent, std::to_string(freePercent)},
                                  {json::gitRevision, (std::string)(GIT_REV)},
                                  {json::gitBranch, (std::string)GIT_BRANCH},
                                  {json::currentRTCTime, std::to_string(static_cast<uint32_t>(std::time(nullptr)))},
                                  {json::version, std::string(VERSION)}}));

        return http::Code::OK;
    }

} // namespace sdesktop::endpoints
