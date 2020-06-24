#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "../ServiceDesktop.hpp"

static sys::ReturnCodes run(uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    std::string fileName = body["fileName"].string_value();

    json11::Json responseBodyJson = json11::Json::object({{parserutils::json::updateReady, true}});

    json11::Json responsePayloadJson =
        json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::update)},
                              {parserutils::json::status, static_cast<int>(parserutils::http::Code::OK)},
                              {parserutils::json::uuid, std::to_string(uuid)},
                              {parserutils::json::body, responseBodyJson}});

    responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

    auto msg = std::make_shared<sdesktop::UpdateOsMessage>(MessageType::UpdateOS, fileName, uuid);
    sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerService);

    return sys::ReturnCodes::Success;
}

static sys::ReturnCodes getUpdates(uint32_t uuid,
                                   json11::Json &body,
                                   std::string &responseStr,
                                   sys::Service *ownerService)
{
    json11::Json fileList = vfs.listdir(purefs::dir::os_updates.c_str(), updateos::extension::update, true);

    json11::Json responseBodyJson = json11::Json::object{{parserutils::json::updateFileList, fileList}};

    json11::Json responsePayloadJson =
        json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::update)},
                              {parserutils::json::status, static_cast<int>(parserutils::http::Code::OK)},
                              {parserutils::json::uuid, std::to_string(uuid)},
                              {parserutils::json::body, responseBodyJson}});

    responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EndpointHandler::update(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::post)) {
        return run(uuid, body, responseStr, ownerService);
    }
    else if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::get)) {
        return getUpdates(uuid, body, responseStr, ownerService);
    }

    return sys::ReturnCodes::Failure;
}
