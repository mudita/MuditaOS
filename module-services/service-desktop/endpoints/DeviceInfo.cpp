#include "EndpointHandler.hpp"
#include <source/version.hpp>
#include <vfs.hpp>
#include <common_data/EventStore.hpp>
#include <time/time_conversion.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <string>

time_t FreeRTOS_time(time_t *pxTime)
{
    return utils::time::Time().getTime();
}

sys::ReturnCodes EndpointHandler::deviceInfo(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(ParserStateMachine::http::Method::get)) {
        vfs::FilesystemStats fsStats = vfs.getFilesystemStats();

        json11::Json responseBodyJson = json11::Json::object(
            {{ParserStateMachine::json::batteryLevel, std::to_string(Store::Battery::get().level)},
             {ParserStateMachine::json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
             {ParserStateMachine::json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
             {ParserStateMachine::json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))},
             {ParserStateMachine::json::signalStrength,
              std::to_string(static_cast<int>(Store::GSM::get()->getSignalStrength().rssiBar))},
             {ParserStateMachine::json::fsTotal, std::to_string(fsStats.totalMbytes)},
             {ParserStateMachine::json::fsFree, std::to_string(fsStats.freeMbytes)},
             {ParserStateMachine::json::fsFreePercent, std::to_string(fsStats.freePercent)},
             {ParserStateMachine::json::gitRevision, (std::string)(GIT_REV)},
             {ParserStateMachine::json::gitTag, (std::string)GIT_TAG},
             {ParserStateMachine::json::gitBranch, (std::string)GIT_BRANCH},
             {ParserStateMachine::json::currentRTCTime,
              std::to_string(static_cast<uint32_t>(utils::time::Time().getTime()))}});

        json11::Json responsePayloadJson = json11::Json::object(
            {{ParserStateMachine::json::endpoint, static_cast<int>(ParserStateMachine::Endpoint::deviceInfo)},
             {ParserStateMachine::json::status, static_cast<int>(ParserStateMachine::http::Code::OK)},
             {ParserStateMachine::json::uuid, std::to_string(uuid)},
             {ParserStateMachine::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        return sys::ReturnCodes::Success;
    }
    else {
        LOG_ERROR("Incorrect method");
        return sys::ReturnCodes::Failure;
    }
}
