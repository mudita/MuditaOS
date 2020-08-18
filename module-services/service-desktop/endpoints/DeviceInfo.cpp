#include "EndpointHandler.hpp"
#include "Endpoint.hpp"
#include <source/version.hpp>
#include <vfs.hpp>
#include <common_data/EventStore.hpp>
#include <time/time_conversion.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <string>

using namespace ParserStateMachine;

time_t FreeRTOS_time(time_t *pxTime)
{
    return utils::time::Time().getTime();
}

// sys::ReturnCodes EndpointHandler::deviceInfo(
//     uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
// {
//     if (httpMethod == static_cast<uint8_t>(ParserStateMachine::http::Method::get)) {
//         vfs::FilesystemStats fsStats = vfs.getFilesystemStats();

//         json11::Json responseBodyJson = json11::Json::object(
//             {{json::batteryLevel, std::to_string(Store::Battery::get().level)},
//              {json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
//              {json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
//              {json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))},
//              {json::signalStrength,
//               std::to_string(static_cast<int>(Store::GSM::get()->getSignalStrength().rssiBar))},
//              {json::fsTotal, std::to_string(fsStats.totalMbytes)},
//              {json::fsFree, std::to_string(fsStats.freeMbytes)},
//              {json::fsFreePercent, std::to_string(fsStats.freePercent)},
//              {json::gitRevision, (std::string)(GIT_REV)},
//              {json::gitTag, (std::string)GIT_TAG},
//              {json::gitBranch, (std::string)GIT_BRANCH},
//              {json::currentRTCTime,
//               std::to_string(static_cast<uint32_t>(utils::time::Time().getTime()))}});

//         json11::Json responsePayloadJson = json11::Json::object(
//             {{json::endpoint, static_cast<int>(EndpointType::deviceInfo)},
//              {json::status, static_cast<int>(http::Code::OK)},
//              {json::uuid, std::to_string(uuid)},
//              {json::body, responseBodyJson}});

//         responseStr = Endpoint::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

//         return sys::ReturnCodes::Success;
//     }
//     else {
//         LOG_ERROR("Incorrect method");
//         return sys::ReturnCodes::Failure;
//     }
// }
