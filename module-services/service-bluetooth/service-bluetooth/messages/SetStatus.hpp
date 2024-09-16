// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class SetStatus : public BluetoothMessage
    {
      public:
        explicit SetStatus(BluetoothStatus status) : status(status)
        {}
        [[nodiscard]] auto getStatus() const noexcept -> BluetoothStatus
        {
            return status;
        }

      private:
        BluetoothStatus status;
    };
} // namespace message::bluetooth
