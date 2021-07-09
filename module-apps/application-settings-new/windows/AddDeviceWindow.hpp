// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "application-settings-new/models/BluetoothSettingsModel.hpp"

#include <Device.hpp>

namespace gui
{
    class AddDeviceWindow : public BaseSettingsWindow
    {
      public:
        explicit AddDeviceWindow(app::Application *app);

      private:
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        auto buildOptionsList() -> std::list<Option> override;

        std::unique_ptr<BluetoothSettingsModel> bluetoothSettingsModel;
        std::vector<Devicei> devices;
    };
}; // namespace gui
