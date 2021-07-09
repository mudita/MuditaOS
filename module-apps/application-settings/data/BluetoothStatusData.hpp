// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-bluetooth/messages/Status.hpp>
#include <SwitchData.hpp>

namespace gui
{
    class BluetoothStatusData : public SwitchData
    {
      public:
        explicit BluetoothStatusData(BluetoothStatus::State state) : state{state}
        {}
        explicit BluetoothStatusData(bool visibility) : visibility{visibility}
        {}
        BluetoothStatusData(BluetoothStatus::State state, bool visibility) : state{state}, visibility{visibility}
        {}

        [[nodiscard]] auto getState() const noexcept -> std::optional<bool>
        {
            if (state == BluetoothStatus::State::On) {
                return true;
            }
            return false;
        }
        [[nodiscard]] auto getVisibility() const noexcept -> std::optional<bool>
        {
            return visibility;
        }

      private:
        std::optional<BluetoothStatus::State> state{};
        std::optional<bool> visibility{};
    };
} // namespace gui
