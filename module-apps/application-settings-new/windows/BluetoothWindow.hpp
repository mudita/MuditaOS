// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
        void switchHandler(bool &switchState);
        void rebuildOptionList();
    };

    class BluetoothStatusData : public SwitchData
    {
      public:
        explicit BluetoothStatusData(BluetoothStatus status) : SwitchData(), status(std::move(status))
        {}
        [[nodiscard]] auto getState() const noexcept -> bool
        {
            if (status.state == BluetoothStatus::State::On) {
                return true;
            }
            return false;
        }
        [[nodiscard]] auto getVisibility() const noexcept -> bool
        {
            return status.visibility;
        }

      private:
        BluetoothStatus status;
    };
} // namespace gui
