#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "version.hpp"
#include "vfs.hpp"
#include <common_data/EventStore.hpp>
#include <string>
#include <time/time_conversion.hpp>
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "../ServiceDesktop.hpp"

sys::ReturnCodes EndpointHandler::update(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::post)) {
        LOG_INFO("update request");
        uint32_t dataSize = static_cast<uint32_t>(body["fileSize"].number_value());
        LOG_INFO("file name: %s", body["fileName"].string_value().c_str());
        LOG_INFO("file size: %d", dataSize);

        json11::Json responseBodyJson =
            json11::Json::object({{"updateReady", true}, {"dataSize", std::to_string(dataSize)}});

        json11::Json responsePayloadJson =
            json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::update)},
                                  {parserutils::json::status, static_cast<int>(parserutils::http::Code::OK)},
                                  {parserutils::json::uuid, std::to_string(uuid)},
                                  {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        ServiceDesktop *service = dynamic_cast<ServiceDesktop *>(ownerService);
        if (service) {
            LOG_INFO("got a pointer to the destkop service, store state");
            service->updateOs(true);
        }
        else {
            LOG_INFO("no valid service handle");
        }

        return sys::ReturnCodes::Success;
    }
    else if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::put)) {
        ServiceDesktop *service = dynamic_cast<ServiceDesktop *>(ownerService);
        if (service) {
            if (service->isUpdating()) {
                LOG_INFO("a chunk of an update file");
                return sys::ReturnCodes::Success;
            }
        }
    }

    return sys::ReturnCodes::Failure;
}
