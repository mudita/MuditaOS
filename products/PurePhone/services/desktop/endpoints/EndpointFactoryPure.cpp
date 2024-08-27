// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
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
#include <endpoints/timeSync/TimeSyncEndpoint.hpp>

namespace sdesktop::endpoints
{
    std::unique_ptr<Endpoint> EndpointFactoryPure::constructEndpoint(Context &context, sys::Service *ownerServicePtr)
    {
        switch (context.getEndpoint()) {
        case EndpointType::Update:
            return std::make_unique<UpdateEndpoint>(ownerServicePtr);
        case EndpointType::FilesystemUpload:
            return FilesystemEndpoint::createInstance(ownerServicePtr);
        case EndpointType::Backup:
            return std::make_unique<BackupEndpoint>(ownerServicePtr);
        case EndpointType::DeviceInfo:
            return std::make_unique<DeviceInfoEndpoint>(ownerServicePtr);
        case EndpointType::Restore:
            return std::make_unique<RestoreEndpoint>(ownerServicePtr);
        case EndpointType::Contacts:
            return std::make_unique<ContactsEndpoint>(ownerServicePtr);
        case EndpointType::Messages:
            return std::make_unique<MessagesEndpoint>(ownerServicePtr);
        case EndpointType::Factory:
            return std::make_unique<FactoryResetEndpoint>(ownerServicePtr);
        case EndpointType::Calllog:
            return std::make_unique<CalllogEndpoint>(ownerServicePtr);
#if ENABLE_DEVELOPER_MODE_ENDPOINT
        case EndpointType::DeveloperMode:
            return std::make_unique<DeveloperModeEndpoint>(ownerServicePtr);
#endif
        case EndpointType::Bluetooth:
            return std::make_unique<BluetoothEndpoint>(ownerServicePtr);
        case EndpointType::UsbSecurity:
            return std::make_unique<SecurityEndpoint>(ownerServicePtr);
        case EndpointType::Outbox:
            return std::make_unique<OutboxEndpoint>(ownerServicePtr);
        case EndpointType::TimeSync:
            return std::make_unique<TimeSyncEndpoint>(ownerServicePtr);
        default:
            return std::make_unique<NullEndpoint>(ownerServicePtr);
        }
    }

    EndpointFactoryPure::EndpointFactoryPure(EndpointSecurity security) : endpointSecurity{security}
    {}

    std::unique_ptr<Endpoint> EndpointFactoryPure::create(Context &context, sys::Service *ownerServicePtr)
    {
        auto security = endpointSecurity;
        if (std::find(whitelist.begin(), whitelist.end(), context.getEndpoint()) != whitelist.end()) {
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
