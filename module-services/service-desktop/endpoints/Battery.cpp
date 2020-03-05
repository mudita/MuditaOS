#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "version.hpp"
#include "vfs.hpp"
#include <common_data/EventStore.hpp>
#include <string>
#include <time/time_conversion.hpp>
#include "service-cellular/api/CellularServiceAPI.hpp"

sys::ReturnCodes EndpointHandler::battery(uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::get))
    {
        //sample json response
        static const int batteryLevel       = Store::Battery::get().level;
        static const int batteryState       = (int)Store::Battery::get().state;

        json11::Json responseBodyJson = json11::Json::object({{"level", (int)batteryLevel}, {"state", batteryState}});

        int statusCode = static_cast<int>(parserutils::http::Code::OK);
        int endpoint = static_cast<int>(parserutils::Endpoint::battery);

        json11::Json responsePayloadJson = json11::Json::object(
            {{parserutils::json::endpoint, endpoint}, {parserutils::json::status, statusCode}, {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        return sys::ReturnCodes::Success;
    }
    else
    {
        LOG_ERROR("Incorrect method");
        return sys::ReturnCodes::Failure;
    }
}