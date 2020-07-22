#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

static bool backupReady = false;

sys::ReturnCodes EndpointHandler::backup(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::post)) {
        json11::Json responseBodyJson;

        if (body[parserutils::json::backupRequest] == true) {
            auto msg = std::make_shared<sdesktop::BackupMessage>();
            sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerService);
            backupReady = true;

            responseBodyJson = json11::Json::object({{parserutils::json::backupRequest, true}});
        }
        else {
            responseBodyJson = json11::Json::object({{parserutils::json::backupRequest, false}});
        }

        json11::Json responsePayloadJson =
            json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::backup)},
                                  {parserutils::json::status, static_cast<int>(parserutils::http::Code::OK)},
                                  {parserutils::json::uuid, std::to_string(uuid)},
                                  {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());
    }
    else if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::get)) {
        json11::Json responseBodyJson;

        if (body[parserutils::json::backupReady] == true) {
            responseBodyJson = json11::Json::object({{parserutils::json::backupReady, backupReady}});
        }

        if (body[parserutils::json::backupUpload] == true) {
            if (vfs.fileExists(purefs::dir::os_backup.c_str()) == true) {
                responseBodyJson = json11::Json::object({{parserutils::json::backupUpload, true}});
            }
            else {
                responseBodyJson = json11::Json::object({{parserutils::json::backupUpload, false}});
            }
        }

        json11::Json responsePayloadJson =
            json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::backup)},
                                  {parserutils::json::status, static_cast<int>(parserutils::http::Code::OK)},
                                  {parserutils::json::uuid, std::to_string(uuid)},
                                  {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());
    }
    else {
        json11::Json responseBodyJson;
        json11::Json responsePayloadJson =
            json11::Json::object({{parserutils::json::endpoint, static_cast<int>(parserutils::Endpoint::backup)},
                                  {parserutils::json::status, static_cast<int>(parserutils::http::Code::BadRequest)},
                                  {parserutils::json::uuid, std::to_string(uuid)},
                                  {parserutils::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());
    }

    return sys::ReturnCodes::Success;
}
