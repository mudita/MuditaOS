#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

sys::ReturnCodes EndpointHandler::factory(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::post)) {
        json11::Json responseBodyJson;

        if (body[parserutils::json::factoryRequest] == true) {
            auto msg = std::make_shared<sdesktop::FactoryMessage>();
            sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerService);

            responseBodyJson = json11::Json::object({{parserutils::json::factoryRequest, true}});
        }
        else {
            responseBodyJson = json11::Json::object({{parserutils::json::factoryRequest, false}});
        }

        json11::Json responsePayloadJson =
            json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::factory)},
                                  {parserutils::json::status, static_cast<int>(parserutils::http::Code::OK)},
                                  {parserutils::json::uuid, std::to_string(uuid)},
                                  {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        return sys::ReturnCodes::Success;
    }
    else {
        json11::Json responseBodyJson = json11::Json::object({{parserutils::json::factoryRequest, false}});

        json11::Json responsePayloadJson =
            json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::factory)},
                                  {parserutils::json::status, static_cast<int>(parserutils::http::Code::BadRequest)},
                                  {parserutils::json::uuid, std::to_string(uuid)},
                                  {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        return sys::ReturnCodes::Failure;
    }
}
