﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeviceInfoEndpoint.hpp"
#include <endpoints/Context.hpp>
#include <parser/MessageHandler.hpp>

#include <common_data/EventStore.hpp>
#include <json/json11.hpp>
#include <source/version.hpp>
#include <time/time_conversion.hpp>
#include <service-desktop/service-desktop/ServiceDesktop.hpp>
#include <cstdint>
#include <string>
#include <sys/statvfs.h>
#include <purefs/filesystem_paths.hpp>

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
    json11::Json updateHistory   = static_cast<ServiceDesktop *>(ownerServicePtr)->updateOS->getUpdateHistory();
    struct statvfs vfstat;
    if (statvfs(purefs::dir::getRootDiskPath().c_str(), &vfstat) < 0) {
        return false;
    }
    auto totalMbytes = (vfstat.f_frsize * vfstat.f_blocks) / 1024LLU / 1024LLU;
    auto freeMbytes  = (vfstat.f_ffree * vfstat.f_bsize) / 1024LLU / 1024LLU;
    auto freePercent = (freeMbytes * 100) / totalMbytes;

    context.setResponseBody(json11::Json::object(
        {{json::batteryLevel, std::to_string(Store::Battery::get().level)},
         {json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
         {json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
         {json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))},
         {json::signalStrength, std::to_string(static_cast<int>(Store::GSM::get()->getSignalStrength().rssiBar))},
         {json::accessTechnology, std::to_string(static_cast<int>(Store::GSM::get()->getNetwork().accessTechnology))},
         {json::networkStatus, std::to_string(static_cast<int>(Store::GSM::get()->getNetwork().status))},
         {json::fsTotal, std::to_string(totalMbytes)},
         {json::fsFree, std::to_string(freeMbytes)},
         {json::fsFreePercent, std::to_string(freePercent)},
         {json::gitRevision, (std::string)(GIT_REV)},
         {json::gitTag, (std::string)GIT_TAG},
         {json::gitBranch, (std::string)GIT_BRANCH},
         {json::updateHistory, updateHistory},
         {json::currentRTCTime, std::to_string(static_cast<uint32_t>(utils::time::getCurrentTimestamp().getTime()))}}));

    MessageHandler::putToSendQueue(context.createSimpleResponse());
    return true;
}
