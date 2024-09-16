// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class Disconnect : public BluetoothMessage
    {};

    class DisconnectResult : public BluetoothMessage
    {
      public:
        explicit DisconnectResult(Devicei device) : device(device)
        {}
        [[nodiscard]] auto getDevice() const -> Devicei
        {
            return device;
        }

      private:
        Devicei device;
    };
} // namespace message::bluetooth
