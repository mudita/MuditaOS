#include "ParserUtils.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"
#include "RestoreEndpoint.hpp"

auto RestoreEndpoint::handle(Context &context) -> std::string
{
    if (context.getMethod() == ParserStateMachine::http::Method::post) {
        json11::Json responseBodyJson;

        if (context.getBody()[ParserStateMachine::json::restoreRequest] == true) {
            auto msg = std::make_shared<sdesktop::RestoreMessage>();
            sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);

            responseBodyJson = json11::Json::object({{ParserStateMachine::json::restoreRequest, true}});
        }
        else {
            responseBodyJson = json11::Json::object({{ParserStateMachine::json::restoreRequest, false}});
        }

        MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
            true, static_cast<int>(EndpointType::restore), context.getUuid(), responseBodyJson));

        return std::string();
    }
    else {
        return std::string();
    }
}
