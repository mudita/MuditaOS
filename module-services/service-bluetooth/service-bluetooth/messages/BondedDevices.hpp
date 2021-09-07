// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class RequestBondedDevices : public BluetoothMessage
    {};

    class ResponseBondedDevices : public BluetoothMessage
    {
      public:
        explicit ResponseBondedDevices(std::vector<Devicei> devices, std::string addressOfConnectedDevice)
            : devices(std::move(devices)), addressOfConnectedDevice(std::move(addressOfConnectedDevice))
        {}
        [[nodiscard]] auto getDevices() const noexcept -> std::vector<Devicei>
        {
            return devices;
        }
        [[nodiscard]] auto getAddressOfConnectedDevice() const noexcept -> const std::string &
        {
            return addressOfConnectedDevice;
        }

      private:
        std::vector<Devicei> devices;
        std::string addressOfConnectedDevice;
    };
} // namespace message::bluetooth
