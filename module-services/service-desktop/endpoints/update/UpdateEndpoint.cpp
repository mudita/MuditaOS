
#include "UpdateEndpoint.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

auto UpdateEndpoint::handle(Context &context) -> std::string
{
    switch (context.getMethod()) {
    case http::Method::post:
        run(context);
        break;
    case http::Method::get:
        getUpdates(context);
        break;
    default:
        break;
    }
    return std::string();
}

auto UpdateEndpoint::run(Context &context) -> sys::ReturnCodes
{
    std::string fileName = context.getBody()["fileName"].string_value();

    json11::Json responseBodyJson = json11::Json::object({{ParserStateMachine::json::updateReady, true}});

    MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
        true, static_cast<int>(EndpointType::update), context.getUuid(), responseBodyJson));

    auto msg = std::make_shared<sdesktop::UpdateOsMessage>(fileName, context.getUuid());
    sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);

    return sys::ReturnCodes::Success;
}

auto UpdateEndpoint::getUpdates(Context &context) -> sys::ReturnCodes
{
    json11::Json fileList = vfs.listdir(purefs::dir::os_updates.c_str(), updateos::extension::update, true);

    json11::Json responseBodyJson = json11::Json::object{{ParserStateMachine::json::updateFileList, fileList}};

    MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
        true, static_cast<int>(EndpointType::update), context.getUuid(), responseBodyJson));

    return sys::ReturnCodes::Success;
}
