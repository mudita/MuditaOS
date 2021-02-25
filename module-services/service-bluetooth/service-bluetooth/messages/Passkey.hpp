// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class RequestPasskey : public BluetoothMessage
    {};

    class ResponsePasskey : public BluetoothMessage
    {
      public:
        explicit ResponsePasskey(std::string passkey) : passkey(std::move(passkey))
        {}
        [[nodiscard]] auto getPasskey() const -> std::string
        {
            return passkey;
        }

      private:
        std::string passkey;
    };
} // namespace message::bluetooth
