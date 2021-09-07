// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Device.hpp>

namespace gui
{

    class BondedDevicesData : public SwitchData
    {
      public:
        explicit BondedDevicesData(std::vector<Devicei> devices, std::string addressOfConnectedDevice)
            : devices(std::move(devices)), addressOfConnectedDevice(std::move(addressOfConnectedDevice))
        {}
        [[nodiscard]] auto getDevices() const noexcept -> const std::vector<Devicei> &
        {
            return devices;
        }
        [[nodiscard]] auto getAddressOfConnectedDevice() const noexcept -> const std::string &
        {
            return addressOfConnectedDevice;
        }

      private:
        const std::vector<Devicei> devices{};
        const std::string addressOfConnectedDevice;
    };
} // namespace gui
