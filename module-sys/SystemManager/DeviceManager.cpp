// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/DeviceManager.hpp>
#include <algorithm>
#include <log/log.hpp>

namespace sys
{

    void DeviceManager::RegisterNewDevice(std::shared_ptr<devices::Device> newDevice)
    {
        if (newDevice) {
            devices.push_back(newDevice);
        }
    }

    void DeviceManager::PrintAllDevices() const noexcept
    {
        std::for_each(std::begin(devices), std::end(devices), PrintName);
    }

    void DeviceManager::DisableAllDevices() const noexcept
    {
        std::for_each(std::begin(devices), std::end(devices), DisableDevice);
    }

    void DeviceManager::PrintName(const DevicePointer &element)
    {
        if (!element.expired()) {
            std::shared_ptr<devices::Device> sharedResource = element.lock();
            LOG_INFO("Device %s", sharedResource->GetName().c_str());
        }
    }

    void DeviceManager::DisableDevice(const DevicePointer &element)
    {
        if (!element.expired()) {
            std::shared_ptr<devices::Device> sharedResource = element.lock();
            if (sharedResource->IsEnabled()) {
                sharedResource->Disable();
                LOG_DEBUG("Device %s has been disabled", sharedResource->GetName().c_str());
            }
        }
    }

} // namespace sys
