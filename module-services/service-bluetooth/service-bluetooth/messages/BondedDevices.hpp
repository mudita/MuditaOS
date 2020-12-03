// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Service/Message.hpp>
#include "service-bluetooth/BluetoothMessage.hpp"
#include <string>

namespace message::bluetooth
{
    class RequestBondedDevices : public BluetoothMessage
    {};

    class ResponseBondedDevices : public BluetoothMessage
    {
      public:
        ResponseBondedDevices(std::vector<Devicei> devices) : devices(std::move(devices))
        {}
        [[nodiscard]] auto getDevices() const -> std::vector<Devicei>
        {
            return devices;
        }

      private:
        std::vector<Devicei> devices;
    };
} // namespace message::bluetooth
