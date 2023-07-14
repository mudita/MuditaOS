// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EndpointFactoryBell.hpp"

#include <endpoints/backup/BackupEndpoint.hpp>
#include <endpoints/deviceInfo/DeviceInfoEndpoint.hpp>
#include <endpoints/factoryReset/FactoryResetEndpoint.hpp>
#include <endpoints/filesystem/FilesystemEndpoint.hpp>
#include <endpoints/nullEndpoint/NullEndpoint.hpp>
#include <endpoints/restore/RestoreEndpoint.hpp>
#include <endpoints/update/UpdateEndpoint.hpp>
#include <endpoints/reboot/RebootEndpoint.hpp>
#include <endpoints/timeSync/TimeSyncEndpoint.hpp>

namespace sdesktop::endpoints
{
    std::unique_ptr<Endpoint> EndpointFactoryBell::create(Context &context, sys::Service *ownerServicePtr)
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
        case EndpointType::Factory:
            return std::make_unique<FactoryResetEndpoint>(ownerServicePtr);
        case EndpointType::Reboot:
            return std::make_unique<RebootEndpoint>(ownerServicePtr);
        case EndpointType::TimeSync:
            return std::make_unique<TimeSyncEndpoint>(ownerServicePtr);
        default:
            return std::make_unique<NullEndpoint>(ownerServicePtr);
        }
    }

    auto EndpointFactory::create([[maybe_unused]] EndpointSecurity endpointSecurity) -> std::unique_ptr<EndpointFactory>
    {
        return std::make_unique<EndpointFactoryBell>();
    }
} // namespace sdesktop::endpoints
