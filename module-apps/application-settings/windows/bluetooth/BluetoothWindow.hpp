// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/bluetooth/BluetoothSettingsModel.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class BluetoothWindow : public BaseSettingsWindow
    {
      public:
        BluetoothWindow(app::ApplicationCommon *app, std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel);

      private:
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto buildOptionsList() -> std::list<Option> override;
        void changeBluetoothState(bool &currentState);
        void changeVisibility(bool &currentVisibility);

        std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel;
        bool isBluetoothSwitchOn            = false;
        bool isPhoneVisibilitySwitchOn      = false;
        OptionWindowDestroyer rai_destroyer = OptionWindowDestroyer(*this);
    };
} // namespace gui
