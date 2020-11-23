// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Endpoint.hpp"

#include "Service/Service.hpp"
#include "backup/BackupEndpoint.hpp"
#include "deviceInfo/DeviceInfoEndpoint.hpp"
#include "deviceUpdate/DeviceUpdateEndpoint.hpp"
#include "update/UpdateEndpoint.hpp"
#include "filesystem/FilesystemEndpoint.hpp"
#include "factoryReset/FactoryResetEndpoint.hpp"
#include "calllog/CalllogEndpoint.hpp"
#include "contacts/ContactsEndpoint.hpp"
#include "developerMode/DeveloperModeEndpoint.hpp"
#include "messages/MessagesEndpoint.hpp"
#include "restore/RestoreEndpoint.hpp"
#include "update/UpdateEndpoint.hpp"

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
        case EndpointType::filesystemUpload:
            return std::make_unique<FilesystemEndpoint>(ownerServicePtr);
        case EndpointType::backup:
            return std::make_unique<BackupEndpoint>(ownerServicePtr);
        case EndpointType::deviceInfo:
            return std::make_unique<DeviceInfoEndpoint>(ownerServicePtr);
        case EndpointType::deviceUpdate:
            return std::make_unique<DeviceUpdateEndpoint>(ownerServicePtr);
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
        case EndpointType::developerMode:
            return std::make_unique<DeveloperModeEndpoint>(ownerServicePtr);
        default:
            return nullptr;
        }
    }
};
