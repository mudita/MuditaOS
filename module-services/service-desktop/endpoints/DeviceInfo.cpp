#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "version.hpp"
#include "vfs.hpp"
#include <common_data/EventStore.hpp>

#include <string>
#include <time/time_conversion.hpp>
#include "../ServiceDesktop.hpp"

time_t FreeRTOS_time(time_t *pxTime)
{
    return utils::time::Time().getTime();
}

sys::ReturnCodes EndpointHandler::deviceInfo(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::get)) {
        vfs::FilesystemStats fsStats = vfs.getFilesystemStats();

        json11::Json responseBodyJson = json11::Json::object(
            {{parserutils::json::batteryLevel, std::to_string(Store::Battery::get().level)},
             {parserutils::json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
             {parserutils::json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
             {parserutils::json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))},
             {parserutils::json::signalStrength,
              std::to_string(static_cast<int>(Store::GSM::get()->getSignalStrength().rssiBar))},
             {parserutils::json::fsTotal, std::to_string(fsStats.totalMbytes)},
             {parserutils::json::fsFree, std::to_string(fsStats.freeMbytes)},
             {parserutils::json::fsFreePercent, std::to_string(fsStats.freePercent)},
             {parserutils::json::gitRevision, (std::string)(GIT_REV)},
             {parserutils::json::gitTag, (std::string)GIT_TAG},
             {parserutils::json::gitBranch, (std::string)GIT_BRANCH},
             {parserutils::json::currentRTCTime,
              std::to_string(static_cast<uint32_t>(utils::time::Time().getTime()))}});

        json11::Json responsePayloadJson =
            json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::deviceInfo)},
                                  {parserutils::json::status, static_cast<int>(parserutils::http::Code::OK)},
                                  {parserutils::json::uuid, std::to_string(uuid)},
                                  {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        return sys::ReturnCodes::Success;
    }
    else {
        LOG_ERROR("Incorrect method");
        return sys::ReturnCodes::Failure;
    }
}
