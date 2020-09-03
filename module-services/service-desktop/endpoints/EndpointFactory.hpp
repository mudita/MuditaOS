#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"
#include "contacts/ContactsEndpoint.hpp"
#include "messages/MessagesEndpoint.hpp"
#include "backup/BackupEndpoint.hpp"
#include "deviceInfo/DeviceInfoEndpoint.hpp"
#include "update/UpdateEndpoint.hpp"
#include "restore/RestoreEndpoint.hpp"
#include "factoryReset/FactoryResetEndpoint.hpp"

using namespace parserFSM;

class EndpointFactory
{
  public:
    static auto create(Context &context, sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint>
    {
        LOG_DEBUG("Creating endpoint: %d", static_cast<int>(context.getEndpoint()));
        switch (context.getEndpoint()) {
        case EndpointType::update:
            return std::make_unique<UpdateEndpoint>(ownerServicePtr);
        case EndpointType::backup:
            return std::make_unique<BackupEndpoint>(ownerServicePtr);
        case EndpointType::deviceInfo:
            return std::make_unique<DeviceInfoEndpoint>(ownerServicePtr);
        case EndpointType::restore:
            return std::make_unique<RestoreEndpoint>(ownerServicePtr);
        case EndpointType::contacts:
            return std::make_unique<ContactsEndpoint>(ownerServicePtr);
        case EndpointType::messages:
            return std::make_unique<MessagesEndpoint>(ownerServicePtr);
        case EndpointType::factory:
            return std::make_unique<FactoryResetEndpoint>(ownerServicePtr);
        default:
            return nullptr;
        }
    }
};
