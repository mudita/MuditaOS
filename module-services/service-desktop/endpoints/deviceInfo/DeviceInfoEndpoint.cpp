// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeviceInfoEndpoint.hpp"

#include "Context.hpp"        // for Context
#include "MessageHandler.hpp" // for MessageHandler

#include <common_data/EventStore.hpp> // for GSM, Battery, SignalStrength
#include <source/version.hpp>         // for GIT_BRANCH, GIT_REV, GIT_TAG
#include <time/time_conversion.hpp>   // for Time
#include <stdint.h>                   // for uint32_t
#include <vfs.hpp>                    // for vfs::FilesystemStats, vfs

#include "json/json11.hpp" // for Json, Json::object
#include <string>          // for to_string, allocator, string

auto DeviceInfoEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::get:
        getDeviceInfo(context);
        break;
    default:
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        break;
    }
}
auto DeviceInfoEndpoint::getDeviceInfo(Context &context) -> bool
{
    vfs::FilesystemStats fsStats = vfs.getFilesystemStats();

    context.setResponseBody(json11::Json::object(
        {{json::batteryLevel, std::to_string(Store::Battery::get().level)},
         {json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
         {json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
         {json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))},
         {json::signalStrength, std::to_string(static_cast<int>(Store::GSM::get()->getSignalStrength().rssiBar))},
         {json::fsTotal, std::to_string(fsStats.totalMbytes)},
         {json::fsFree, std::to_string(fsStats.freeMbytes)},
         {json::fsFreePercent, std::to_string(fsStats.freePercent)},
         {json::gitRevision, (std::string)(GIT_REV)},
         {json::gitTag, (std::string)GIT_TAG},
         {json::gitBranch, (std::string)GIT_BRANCH},
         {json::currentRTCTime, std::to_string(static_cast<uint32_t>(utils::time::Time().getTime()))}}));

    MessageHandler::putToSendQueue(context.createSimpleResponse());
    return true;
}
