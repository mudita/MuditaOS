// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class Connect : public BluetoothMessage
    {
      public:
        explicit Connect(Devicei device) : device(std::move(device))
        {}
        [[nodiscard]] auto getDevice() const -> Devicei
        {
            return device;
        }

      private:
        Devicei device;
    };

    class ConnectResult : public BluetoothMessage
    {
      public:
        explicit ConnectResult(Devicei device, bool succeed) : device(device), succeed(succeed)
        {}
        [[nodiscard]] auto getDevice() const -> Devicei
        {
            return device;
        }
        [[nodiscard]] auto isSucceed() const noexcept -> bool
        {
            return succeed;
        }

      private:
        Devicei device;
        bool succeed;
    };
} // namespace message::bluetooth
