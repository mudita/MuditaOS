// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Endpoint.hpp"

#include "Service/Service.hpp"
#include "calendarEvents/CalendarEventsEndpoint.hpp"
#include "backup/BackupEndpoint.hpp"
#include "deviceInfo/DeviceInfoEndpoint.hpp"
#include "update/UpdateEndpoint.hpp"
#include "filesystem/FilesystemEndpoint.hpp"
#include "factoryReset/FactoryResetEndpoint.hpp"
#include "calllog/CalllogEndpoint.hpp"
#include "contacts/ContactsEndpoint.hpp"
#include "developerMode/DeveloperModeEndpoint.hpp"
#include "messages/MessagesEndpoint.hpp"
#include "restore/RestoreEndpoint.hpp"
#include "update/UpdateEndpoint.hpp"

class EndpointFactory
{
  public:
    static auto create(parserFSM::Context &context, sys::Service *ownerServicePtr)
        -> std::unique_ptr<parserFSM::Endpoint>
    {
        LOG_DEBUG("Creating endpoint: %d", static_cast<int>(context.getEndpoint()));
        switch (context.getEndpoint()) {
        case parserFSM::EndpointType::update:
            return std::make_unique<UpdateEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::filesystemUpload:
            return std::make_unique<FilesystemEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::backup:
            return std::make_unique<BackupEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::deviceInfo:
            return std::make_unique<DeviceInfoEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::restore:
            return std::make_unique<RestoreEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::contacts:
            return std::make_unique<ContactsEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::messages:
            return std::make_unique<MessagesEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::factory:
            return std::make_unique<FactoryResetEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::calllog:
            return std::make_unique<CalllogEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::developerMode:
            return std::make_unique<DeveloperModeEndpoint>(ownerServicePtr);
        case parserFSM::EndpointType::calendarEvents:
            return std::make_unique<CalendarEventsEndpoint>(ownerServicePtr);
        default:
            return nullptr;
        }
    }
};
