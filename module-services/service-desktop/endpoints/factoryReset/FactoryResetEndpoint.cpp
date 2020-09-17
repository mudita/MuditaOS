#include "FactoryResetEndpoint.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

auto FactoryResetEndpoint::handle(Context &context) -> void
{
    if (context.getMethod() == parserFSM::http::Method::post) {

        if (context.getBody()[parserFSM::json::factoryRequest] == true) {
            auto msg = std::make_shared<sdesktop::FactoryMessage>();
            sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);

            context.setResponseBody(json11::Json::object({{parserFSM::json::factoryRequest, true}}));
        }
        else {
            context.setResponseBody(json11::Json::object({{parserFSM::json::factoryRequest, false}}));
        }

        MessageHandler::putToSendQueue(context.createSimpleResponse());

        return;
    }
    else {
        context.setResponseBody(json11::Json::object({{parserFSM::json::factoryRequest, false}}));

        MessageHandler::putToSendQueue(context.createSimpleResponse());

        return;
    }
}
