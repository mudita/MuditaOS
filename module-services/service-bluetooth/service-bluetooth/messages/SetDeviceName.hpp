// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{

    class SetDeviceName : public BluetoothMessage
    {
      public:
        explicit SetDeviceName(std::string name) : name(std::move(name))
        {}
        [[nodiscard]] auto getName() const -> std::string
        {
            return name;
        }

      private:
        std::string name;
    };
} // namespace message::bluetooth
