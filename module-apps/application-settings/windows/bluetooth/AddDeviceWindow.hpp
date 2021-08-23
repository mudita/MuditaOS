// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/bluetooth/BluetoothSettingsModel.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

#include <Device.hpp>

namespace gui
{
    class AddDeviceWindow : public BaseSettingsWindow
    {
      public:
        AddDeviceWindow(app::Application *app, std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel);

      private:
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        auto buildOptionsList() -> std::list<Option> override;

        std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel{};
        std::vector<Devicei> devices;
    };
}; // namespace gui
