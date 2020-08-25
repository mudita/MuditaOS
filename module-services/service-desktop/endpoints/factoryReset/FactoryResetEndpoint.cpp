#include "FactoryResetEndpoint.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

auto FactoryResetEndpoint::handle(Context &context) -> std::string
{
    if (context.getMethod() == ParserStateMachine::http::Method::post) {
        json11::Json responseBodyJson;

        if (context.getBody()[ParserStateMachine::json::factoryRequest] == true) {
            auto msg = std::make_shared<sdesktop::FactoryMessage>();
            sys::Bus::SendUnicast(msg, service::name::service_desktop, ownerServicePtr);

            responseBodyJson = json11::Json::object({{ParserStateMachine::json::factoryRequest, true}});
        }
        else {
            responseBodyJson = json11::Json::object({{ParserStateMachine::json::factoryRequest, false}});
        }

        MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
            sys::ReturnCodes::Success, static_cast<int>(EndpointType::factory), context.getUuid(), json11::Json()));

        return std::string();
    }
    else {
        json11::Json responseBodyJson = json11::Json::object({{ParserStateMachine::json::factoryRequest, false}});

        MessageHandler::putToSendQueue(Endpoint::createSimpleResponse(
            sys::ReturnCodes::Failure, static_cast<int>(EndpointType::factory), context.getUuid(), json11::Json()));

        return std::string();
    }
}
