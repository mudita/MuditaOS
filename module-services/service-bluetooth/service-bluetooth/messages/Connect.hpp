// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
        enum class Result
        {
            Success,
            Failure
        };

        explicit ConnectResult(Devicei device, Result result) : device(std::move(device)), result(result)
        {}
        [[nodiscard]] auto getDevice() const -> Devicei
        {
            return device;
        }
        [[nodiscard]] auto getResult() const noexcept -> Result
        {
            return result;
        }

      private:
        Devicei device;
        Result result;
    };
} // namespace message::bluetooth
