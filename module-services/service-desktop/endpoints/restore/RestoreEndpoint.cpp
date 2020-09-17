#include "ParserUtils.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"
#include "RestoreEndpoint.hpp"

auto RestoreEndpoint::handle(Context &context) -> void
{
    if (context.getMethod() == parserFSM::http::Method::post) {

        if (context.getBody()[parserFSM::json::restoreRequest] == true) {
            auto msg = std::make_shared<sdesktop::RestoreMessage>();
            sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);

            context.setResponseBody(json11::Json::object({{parserFSM::json::restoreRequest, true}}));
        }
        else {
            context.setResponseBody(json11::Json::object({{parserFSM::json::restoreRequest, false}}));
        }

        MessageHandler::putToSendQueue(context.createSimpleResponse());

        return;
    }
    else {
        return;
    }
}
