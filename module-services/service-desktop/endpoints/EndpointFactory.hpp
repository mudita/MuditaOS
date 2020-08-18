#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"
#include "contacts/ContactsEndpoint.hpp"
#include "messages/MessagesEndpoint.hpp"

using namespace ParserStateMachine;

class EndpointFactory
{
  public:
    static auto create(Context &context, sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint>
    {
        LOG_DEBUG("Creating endpoint: %d", static_cast<int>(context.getEndpoint()));
        switch (context.getEndpoint()) {
        // case EndpointType::deviceInfo:
        //     return std::make_unique(EndpointDeviceInfo());
        // case EndpointType::update:
        //     return std::make_unique(EndpointUpdate());
        // case EndpointType::backup:
        //     return std::make_unique(EndpointBackup());
        // case EndpointType::restore:
        //     return std::make_unique(EndpointRestore());
        case EndpointType::contacts:
            return std::make_unique<ContactsEndpoint>(ownerServicePtr);
        case EndpointType::messages:
            return std::make_unique<MessagesEndpoint>(ownerServicePtr);
        default:
            return nullptr;
        }
    }
};
