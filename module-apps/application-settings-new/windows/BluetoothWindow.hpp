// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionWindow.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>

namespace gui
{
    class BluetoothWindow : public OptionWindow
    {
      public:
        BluetoothWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        bool isBluetoothSwitchOn       = false;
        bool isPhoneVisibilitySwitchOn = false;
        auto bluetoothOptionsList() -> std::list<gui::Option>;
        void changeBluetoothState(bool currentState);
        void changeVisibility(bool currentVisibility);
        void rebuildOptionList();

        static BluetoothStatus makeDesiredStatus(bool desiredBluetoothState, bool desiredVisibility) noexcept;
    };

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
            if (!state.has_value()) {
                return std::nullopt;
            }
            if (state == BluetoothStatus::State::On) {
                return true;
            }
            return false;
        }
        [[nodiscard]] auto getVisibility() const noexcept -> std::optional<bool>
        {
            if (!visibility.has_value()) {
                return std::nullopt;
            }
            return visibility;
        }

      private:
        std::optional<BluetoothStatus::State> state;
        std::optional<bool> visibility;
    };
} // namespace gui
