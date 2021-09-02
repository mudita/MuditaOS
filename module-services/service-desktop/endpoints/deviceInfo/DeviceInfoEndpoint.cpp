// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeviceInfoEndpoint.hpp"
#include <endpoints/Context.hpp>
#include <parser/MessageHandler.hpp>

#include <EventStore.hpp>
#include <source/version.hpp>
#include <service-desktop/service-desktop/ServiceDesktop.hpp>
#include <version.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <sys/statvfs.h>
#include <purefs/filesystem_paths.hpp>

#include <ctime>

using namespace parserFSM;

auto DeviceInfoEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::get:
        handleGet(context);
        break;
    default:
        context.setResponseStatus(http::Code::BadRequest);
        break;
    }

    MessageHandler::putToSendQueue(context.createSimpleResponse());
}

auto DeviceInfoEndpoint::handleGet(Context &context) -> void
{
    const auto &requestBody = context.getBody();

    if (requestBody.is_null()) {
        context.setResponseStatus(parserFSM::http::Code::NotFound);
    }

    if (requestBody[json::fileList].is_number()) {

        auto diagFileList = static_cast<DiagnosticsFileList>(requestBody[json::fileList].int_value());

        if (!magic_enum::enum_contains<DiagnosticsFileList>(diagFileList)) {
            LOG_ERROR("Bad diagnostic type %d requested", static_cast<unsigned>(diagFileList));

            context.setResponseStatus(parserFSM::http::Code::BadRequest);
            return;
        }

        gatherListOfDiagnostics(context, diagFileList);
    }
    else {
        getDeviceInfo(context);
    }
}

auto DeviceInfoEndpoint::gatherListOfDiagnostics(Context &context, DiagnosticsFileList diagFileList) -> void
{
    std::vector<std::string> fileList;
    auto status = http::Code::NoContent;

    try {
        requestLogsFlush();
    }
    catch (const std::runtime_error &e) {
        LOG_ERROR("Logs flush exception: %s", e.what());
    }

    switch (diagFileList) {
    case DiagnosticsFileList::LOGS: {
        fileList = listDirectory(purefs::dir::getLogsPath());
        break;
    }

    case DiagnosticsFileList::CRASH_DUMPS: {
        fileList = listDirectory(purefs::dir::getCrashDumpsPath());
        break;
    }
    }

    if (!fileList.empty()) {
        status = http::Code::OK;
        context.setResponseBody(fileListToJsonObject(fileList));
    }

    context.setResponseStatus(status);
}

auto DeviceInfoEndpoint::requestLogsFlush() const -> void
{
    auto owner = dynamic_cast<ServiceDesktop *>(ownerServicePtr);
    if (owner) {
        owner->requestLogsFlush();
    }
}

auto DeviceInfoEndpoint::fileListToJsonObject(const std::vector<std::string> &fileList) const
    -> json11::Json::object const
{
    json11::Json::array fileArray;

    for (const auto &file : fileList) {
        fileArray.push_back(file);
    }

    return json11::Json::object{{json::files, fileArray}};
}

auto DeviceInfoEndpoint::listDirectory(std::string path) -> std::vector<std::string>
{
    std::vector<std::string> entries;

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        entries.push_back(entry.path());
    }

    return entries;
}

auto DeviceInfoEndpoint::getSerialNumber() -> std::string
{
    return dynamic_cast<ServiceDesktop *>(ownerServicePtr)->getSerialNumber();
}

auto DeviceInfoEndpoint::getDeviceInfo(Context &context) -> bool
{
    if (ownerServicePtr == nullptr) {
        return false;
    }
    std::unique_ptr<struct statvfs> vfstat = std::make_unique<struct statvfs>();
    if ((*statvfs)(purefs::dir::getRootDiskPath().c_str(), vfstat.get()) < 0) {
        return false;
    }

    unsigned long totalMbytes = (vfstat->f_frsize * vfstat->f_blocks) / 1024LLU / 1024LLU;
    unsigned long freeMbytes  = (vfstat->f_bfree * vfstat->f_bsize) / 1024LLU / 1024LLU;
    unsigned long freePercent = (freeMbytes * 100) / totalMbytes;

    context.setResponseBody(json11::Json::object(
        {{json::batteryLevel, std::to_string(Store::Battery::get().level)},
         {json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
         {json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
         {json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))},
         {json::signalStrength, std::to_string(static_cast<int>(Store::GSM::get()->getSignalStrength().rssiBar))},
         {json::accessTechnology, std::to_string(static_cast<int>(Store::GSM::get()->getNetwork().accessTechnology))},
         {json::networkStatus, std::to_string(static_cast<int>(Store::GSM::get()->getNetwork().status))},
         {json::networkOperatorName, Store::GSM::get()->getNetworkOperatorName()},
         {json::fsTotal, std::to_string(totalMbytes)},
         {json::fsFree, std::to_string(freeMbytes)},
         {json::fsFreePercent, std::to_string(freePercent)},
         {json::gitRevision, (std::string)(GIT_REV)},
         {json::gitTag, (std::string)GIT_TAG},
         {json::gitBranch, (std::string)GIT_BRANCH},
         {json::currentRTCTime, std::to_string(static_cast<uint32_t>(std::time(nullptr)))},
         {json::version, std::string(VERSION)},
         {json::serialNumber, getSerialNumber()}}));

    return true;
}
