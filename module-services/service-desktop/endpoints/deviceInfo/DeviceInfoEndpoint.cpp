#include "DeviceInfoEndpoint.hpp"
#include <source/version.hpp>
#include <vfs.hpp>
#include <common_data/EventStore.hpp>
#include <time/time_conversion.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <string>

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
