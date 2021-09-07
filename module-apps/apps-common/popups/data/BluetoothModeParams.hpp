// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/Popups.hpp"

#include <service-appmgr/Actions.hpp>
#include <module-sys/PhoneModes/Common.hpp>

namespace gui
{
    class BluetoothModeParams : public app::manager::actions::ActionParams
    {
      public:
        explicit BluetoothModeParams(sys::bluetooth::BluetoothMode mode) : bluetoothMode{mode}
        {}

        [[nodiscard]] auto getBluetoothMode() const noexcept
        {
            return bluetoothMode;
        }

      private:
        sys::bluetooth::BluetoothMode bluetoothMode;
    };
} // namespace gui
