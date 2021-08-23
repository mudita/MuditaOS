// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class Unpair : public BluetoothMessage
    {
      public:
        explicit Unpair(Devicei device) : device(device)
        {}
        [[nodiscard]] auto getDevice() const -> Devicei
        {
            return device;
        }

      private:
        Devicei device;
    };

    class UnpairResult : public BluetoothMessage
    {
      public:
        explicit UnpairResult(Devicei device, bool succeed) : device(device), succeed(succeed)
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
