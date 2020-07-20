#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

static bool backupReady = false;

static sys::ReturnCodes request(uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    json11::Json responseBodyJson;

    if (body[ParserStateMachine::json::backupRequest] == true) {
        auto msg = std::make_shared<sdesktop::BackupMessage>();
        sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerService);
        backupReady = true;

        responseBodyJson = json11::Json::object({{ParserStateMachine::json::backupRequest, true}});
    }
    else {
        responseBodyJson = json11::Json::object({{ParserStateMachine::json::backupRequest, false}});
    }

    json11::Json responsePayloadJson = json11::Json::object(
        {{ParserStateMachine::json::endpoint, static_cast<int>(ParserStateMachine::Endpoint::backup)},
         {ParserStateMachine::json::status, static_cast<int>(ParserStateMachine::http::Code::OK)},
         {ParserStateMachine::json::uuid, std::to_string(uuid)},
         {ParserStateMachine::json::body, responseBodyJson}});

    responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

    return sys::ReturnCodes::Success;
}

static sys::ReturnCodes upload(uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    json11::Json responseBodyJson;

    if (body[ParserStateMachine::json::backupReady] == true) {
        responseBodyJson = json11::Json::object({{ParserStateMachine::json::backupReady, backupReady}});
    }
    else if (body[ParserStateMachine::json::backupUpload] == true) {
        if (vfs.fileExists(purefs::dir::os_backup.c_str()) == true) {
            responseBodyJson = json11::Json::object({{ParserStateMachine::json::backupUpload, true}});
        }
        else {
            responseBodyJson = json11::Json::object({{ParserStateMachine::json::backupUpload, false}});
        }
    }
    else {
        responseBodyJson = json11::Json::object({{ParserStateMachine::json::backupReady, false}});
    }

    json11::Json responsePayloadJson = json11::Json::object(
        {{ParserStateMachine::json::endpoint, static_cast<int>(ParserStateMachine::Endpoint::backup)},
         {ParserStateMachine::json::status, static_cast<int>(ParserStateMachine::http::Code::OK)},
         {ParserStateMachine::json::uuid, std::to_string(uuid)},
         {ParserStateMachine::json::body, responseBodyJson}});

    responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EndpointHandler::backup(
    uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(ParserStateMachine::http::Method::post)) {
        return request(uuid, body, responseStr, ownerService);
    }
    else if (httpMethod == static_cast<uint8_t>(ParserStateMachine::http::Method::get)) {
        return upload(uuid, body, responseStr, ownerService);
    }
    else {
        json11::Json responseBodyJson;
        json11::Json responsePayloadJson = json11::Json::object(
            {{ParserStateMachine::json::endpoint, static_cast<int>(ParserStateMachine::Endpoint::backup)},
             {ParserStateMachine::json::status, static_cast<int>(ParserStateMachine::http::Code::BadRequest)},
             {ParserStateMachine::json::uuid, std::to_string(uuid)},
             {ParserStateMachine::json::body, responseBodyJson}});

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        return sys::ReturnCodes::Failure;
    }
}
