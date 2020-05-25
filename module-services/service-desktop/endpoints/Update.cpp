#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "../ServiceDesktop.hpp"

sys::ReturnCodes EndpointHandler::update(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::post)) {
        LOG_INFO("update request: ");
        LOG_INFO("%s", body.dump().c_str());
        std::string fileName = body["fileName"].string_value();

        LOG_INFO("update request, fileName: %s", fileName.c_str());

        json11::Json responseBodyJson = json11::Json::object({{parserutils::json::updateReady, true}});

        json11::Json responsePayloadJson =
            json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::update)},
                                  {parserutils::json::status, static_cast<int>(parserutils::http::Code::OK)},
                                  {parserutils::json::uuid, std::to_string(uuid)},
                                  {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        auto msg = std::make_shared<sdesktop::UpdateOsMessage>(MessageType::UpdateOS, fileName, uuid);
        sys::Bus::SendUnicast(msg, ServiceDesktop::serviceName, ownerService);

        return sys::ReturnCodes::Success;
    }
    else if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::get)) {
        LOG_INFO("update get request: ");
        LOG_INFO("%s", body.dump().c_str());

        json11::Json fileList = vfs.listdir(PATH_UPDATES, updateos::extension::update);

        json11::Json responseBodyJson = json11::Json::object{{parserutils::json::updateFileList, fileList}};

        json11::Json responsePayloadJson =
            json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::update)},
                                  {parserutils::json::status, static_cast<int>(parserutils::http::Code::OK)},
                                  {parserutils::json::uuid, std::to_string(uuid)},
                                  {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        return sys::ReturnCodes::Success;
    }
    else if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::put)) {
        return sys::ReturnCodes::Failure;
    }
    return sys::ReturnCodes::Failure;
}
