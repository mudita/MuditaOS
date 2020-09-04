
#include "UpdateEndpoint.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

auto UpdateEndpoint::handle(Context &context) -> void
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
}

auto UpdateEndpoint::run(Context &context) -> sys::ReturnCodes
{
    std::string fileName = context.getBody()["fileName"].string_value();

    context.setResponseBody(json11::Json::object({{parserFSM::json::updateReady, true}}));

    MessageHandler::putToSendQueue(context.createSimpleResponse());

    auto msg = std::make_shared<sdesktop::UpdateOsMessage>(fileName, context.getUuid());
    sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);

    return sys::ReturnCodes::Success;
}

auto UpdateEndpoint::getUpdates(Context &context) -> sys::ReturnCodes
{
    json11::Json fileList = vfs.listdir(purefs::dir::os_updates.c_str(), updateos::extension::update, true);

    context.setResponseBody(json11::Json::object{{parserFSM::json::updateFileList, fileList}});

    MessageHandler::putToSendQueue(context.createSimpleResponse());

    return sys::ReturnCodes::Success;
}
