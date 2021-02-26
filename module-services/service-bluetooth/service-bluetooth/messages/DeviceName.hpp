// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class RequestDeviceName : public BluetoothMessage
    {};

    class ResponseDeviceName : public BluetoothMessage
    {
      public:
        explicit ResponseDeviceName(std::string name) : name(std::move(name))
        {}
        [[nodiscard]] auto getName() const noexcept -> std::string
        {
            return name;
        }

      private:
        std::string name;
    };
} // namespace message::bluetooth
