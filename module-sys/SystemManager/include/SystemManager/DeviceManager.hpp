// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <vector>
#include "devices/Device.hpp"

namespace sys
{
    using DevicePointer = std::weak_ptr<devices::Device>;
    using DeviceVector  = std::vector<DevicePointer>;

    class DeviceManager
    {

      public:
        void RegisterNewDevice(std::shared_ptr<devices::Device> newDevice);
        void PrintAllDevices() const noexcept;
        void DisableAllDevices() const noexcept;

      private:
        static void PrintName(const DevicePointer &element);
        static void DisableDevice(const DevicePointer &element);

        DeviceVector devices;
    };

} // namespace sys
