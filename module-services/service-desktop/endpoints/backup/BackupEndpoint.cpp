#include "BackupEndpoint.hpp"
#include "ParserUtils.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

static bool backupReady = false;

auto BackupEndpoint::handle(Context &context) -> std::string
{
    switch (context.getMethod()) {
    case http::Method::get:
        upload(context);
        break;
    case http::Method::post:
        request(context);
        break;
    case http::Method::put:
        [[fallthrough]];
    case http::Method::del:
        MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
            sys::ReturnCodes::Failure, static_cast<int>(EndpointType::backup), context.getUuid(), json11::Json()));
        break;
    }

    return std::string();
}
auto BackupEndpoint::request(Context &context) -> sys::ReturnCodes
{
    json11::Json responseBodyJson;

    if (context.getBody()[json::backupRequest] == true) {
        auto msg = std::make_shared<sdesktop::BackupMessage>();
        sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);
        backupReady = true;

        responseBodyJson = json11::Json::object({{json::backupRequest, true}});
    }
    else {
        responseBodyJson = json11::Json::object({{json::backupRequest, false}});
    }

    MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
        true, static_cast<int>(EndpointType::backup), context.getUuid(), responseBodyJson));

    return sys::ReturnCodes::Success;
}

auto BackupEndpoint::upload(Context &context) -> sys::ReturnCodes
{
    json11::Json responseBodyJson;

    if (context.getBody()[json::backupReady] == true) {
        responseBodyJson = json11::Json::object({{json::backupReady, backupReady}});
    }
    else if (context.getBody()[json::backupUpload] == true) {
        if (vfs.fileExists(purefs::dir::os_backup.c_str())) {
            responseBodyJson = json11::Json::object({{json::backupUpload, true}});
        }
        else {
            responseBodyJson = json11::Json::object({{json::backupUpload, false}});
        }
    }
    else {
        responseBodyJson = json11::Json::object({{json::backupReady, false}});
    }

    MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
        true, static_cast<int>(EndpointType::backup), context.getUuid(), responseBodyJson));

    return sys::ReturnCodes::Success;
}
