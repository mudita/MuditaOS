// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <service-bluetooth/Constants.hpp>

namespace sys::bluetooth
{
    class BluetoothModeChanged : public DataMessage
    {
      public:
        explicit BluetoothModeChanged(BluetoothMode bluetoothMode)
            : DataMessage{MessageType::MessageTypeUninitialized}, bluetoothMode{bluetoothMode}
        {}

        [[nodiscard]] auto getBluetoothMode() const noexcept
        {
            return bluetoothMode;
        }

      private:
        BluetoothMode bluetoothMode;
    };
} // namespace sys::bluetooth
