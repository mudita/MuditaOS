// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EndpointFactoryPure.hpp"

#include <endpoints/backup/BackupEndpoint.hpp>
#include <endpoints/bluetooth/BluetoothEndpoint.hpp>
#include <endpoints/calllog/CalllogEndpoint.hpp>
#include <endpoints/contacts/ContactsEndpoint.hpp>
#include <endpoints/developerMode/DeveloperModeEndpoint.hpp>
#include <endpoints/deviceInfo/DeviceInfoEndpoint.hpp>
#include <endpoints/factoryReset/FactoryResetEndpoint.hpp>
#include <endpoints/filesystem/FilesystemEndpoint.hpp>
#include <endpoints/messages/MessagesEndpoint.hpp>
#include <endpoints/outbox/OutboxEndpoint.hpp>
#include <endpoints/nullEndpoint/NullEndpoint.hpp>
#include <endpoints/restore/RestoreEndpoint.hpp>
#include <endpoints/security/SecurityEndpoint.hpp>
#include <endpoints/update/UpdateEndpoint.hpp>

namespace sdesktop::endpoints
{

    std::unique_ptr<Endpoint> EndpointFactoryPure::constructEndpoint(Context &context, sys::Service *ownerServicePtr)
    {
        LOG_DEBUG("Creating endpoint: %d", static_cast<int>(context.getEndpoint()));
        switch (context.getEndpoint()) {
        case EndpointType::update:
            return std::make_unique<UpdateEndpoint>(ownerServicePtr);
        case EndpointType::filesystemUpload:
            return FilesystemEndpoint::createInstance(ownerServicePtr);
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
        case EndpointType::calllog:
            return std::make_unique<CalllogEndpoint>(ownerServicePtr);
#if ENABLE_DEVELOPER_MODE_ENDPOINT
        case EndpointType::developerMode:
            return std::make_unique<DeveloperModeEndpoint>(ownerServicePtr);
#endif
        case EndpointType::bluetooth:
            return std::make_unique<BluetoothEndpoint>(ownerServicePtr);
        case EndpointType::usbSecurity:
            return std::make_unique<SecurityEndpoint>(ownerServicePtr);
        case EndpointType::outbox:
            return std::make_unique<OutboxEndpoint>(ownerServicePtr);
        default:
            return std::make_unique<NullEndpoint>(ownerServicePtr);
        }
    }

    EndpointFactoryPure::EndpointFactoryPure(EndpointSecurity security) : EndpointFactory{}, endpointSecurity{security}
    {}

    std::unique_ptr<Endpoint> EndpointFactoryPure::create(Context &context, sys::Service *ownerServicePtr)
    {
        auto security = endpointSecurity;
        if (std::find(Whitelist.begin(), Whitelist.end(), context.getEndpoint()) != Whitelist.end()) {
            security = EndpointSecurity::Allow;
        }

        switch (security) {
        case EndpointSecurity::Allow:
            return constructEndpoint(context, ownerServicePtr);
        default:
            return std::make_unique<SecuredEndpoint>(ownerServicePtr);
        }
    }

    auto EndpointFactory::create(EndpointSecurity security) -> std::unique_ptr<EndpointFactory>
    {
        return std::make_unique<EndpointFactoryPure>(security);
    }

} // namespace sdesktop::endpoints
