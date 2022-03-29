// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EndpointFactoryBell.hpp"

#include <endpoints/backup/BackupEndpoint.hpp>
#include <endpoints/deviceInfo/DeviceInfoEndpoint.hpp>
#include <endpoints/factoryReset/FactoryResetEndpoint.hpp>
#include <endpoints/filesystem/FilesystemEndpoint.hpp>
#include <endpoints/nullEndpoint/NullEndpoint.hpp>
#include <endpoints/restore/RestoreEndpoint.hpp>
#include <endpoints/update/UpdateEndpoint.hpp>
#include <log/log.hpp>

namespace sdesktop::endpoints
{

    EndpointFactoryBell::EndpointFactoryBell() : EndpointFactory{}
    {}

    std::unique_ptr<Endpoint> EndpointFactoryBell::create(Context &context, sys::Service *ownerServicePtr)
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
        case EndpointType::factory:
            return std::make_unique<FactoryResetEndpoint>(ownerServicePtr);
        default:
            return std::make_unique<NullEndpoint>(ownerServicePtr);
        }
    }

    auto EndpointFactory::create(EndpointSecurity /*unused*/) -> std::unique_ptr<EndpointFactory>
    {
        return std::make_unique<EndpointFactoryBell>();
    }

} // namespace sdesktop::endpoints
