#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "version.hpp"
#include "vfs.hpp"
#include <common_data/EventStore.hpp>
#include <string>
#include <time/time_conversion.hpp>
#include "service-cellular/api/CellularServiceAPI.hpp"

time_t FreeRTOS_time( time_t *pxTime )
{
    return (utils::time::Time().getTime());
}

sys::ReturnCodes EndpointHandler::deviceInfo(uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::get))
    {
        LOG_DEBUG("deviceInfo uuid: %d ownerService: %p", uuid, (void *) ownerService);
        //sample json response
        static const std::string batteryLevel       = std::to_string(Store::Battery::get().level);
        static const std::string batteryState       = std::to_string((int)Store::Battery::get().state);
        static const std::string selectedSim        = std::to_string((int)Store::GSM::get()->selected);
        static const std::string trayState          = std::to_string((int)Store::GSM::get()->tray);
        extern class vfs vfs;
        std::string currentTime                     = std::to_string((uint32_t)utils::time::Time().getTime());
        static const std::string ownNumber          = CellularServiceAPI::GetOwnNumber(ownerService);

        vfs::FilesystemStats fsStats = vfs.getFilesystemStats();

        json11::Json responseBodyJson = json11::Json::object({
                                                                 {"batteryLevel", batteryLevel},
                                                                 {"batteryState", batteryState},
                                                                 { "selectedSim" , selectedSim},
                                                                 { "trayState", trayState},
                                                                 { "fsTotal", std::to_string(fsStats.totalMbytes) },
                                                                 { "fsFree",  std::to_string(fsStats.freeMbytes) },
                                                                 { "fsPercentFree",  std::to_string(fsStats.freePercent) },
                                                                 { "gitRevision", std::string(GIT_REV) },
                                                                 { "gitTag", (std::string)GIT_TAG},
                                                                 {"gitBranch", (std::string)GIT_BRANCH},
                                                                 {"currentRTCTime", currentTime},
                                                                 {"ownNumber", ownNumber }
                                                             });

        // LOG_DEBUG("collected info: %s", responseBodyJson.dump().c_str());

        int statusCode = static_cast<int>(parserutils::http::Code::OK);
        int endpoint = static_cast<int>(parserutils::Endpoint::deviceInfo);

        json11::Json responsePayloadJson = json11::Json::object({
                                                                    {parserutils::json::endpoint, endpoint},
                                                                    {parserutils::json::status, statusCode},
                                                                    {parserutils::json::uuid, std::to_string(uuid)},
                                                                    {parserutils::json::body, responseBodyJson}
                                                                });

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        return sys::ReturnCodes::Success;
    }
    else
    {
        LOG_ERROR("Incorrect method");
        return sys::ReturnCodes::Failure;
    }
}