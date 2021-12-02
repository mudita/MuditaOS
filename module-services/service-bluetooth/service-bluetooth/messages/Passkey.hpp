// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class RequestPasskey : public BluetoothMessage
    {
        Devicei device;

      public:
        explicit RequestPasskey(const Devicei &dev) : device(dev)
        {}

        [[nodiscard]] auto getDevice()
        {
            return device;
        }
    };

    class ResponsePasskey : public BluetoothMessage
    {
        Devicei device;

      public:
        ResponsePasskey(std::string passkey, const Devicei &dev) : device(dev), passkey(std::move(passkey))
        {}

        [[nodiscard]] auto getPasskey() const -> std::string
        {
            return passkey;
        }

        [[nodiscard]] auto getDevice()
        {
            return device;
        }

      private:
        std::string passkey;
    };
} // namespace message::bluetooth
