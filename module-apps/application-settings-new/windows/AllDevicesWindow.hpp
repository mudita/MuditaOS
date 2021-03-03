// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "application-settings-new/models/BluetoothSettingsModel.hpp"
#include "BaseSettingsWindow.hpp"

#include <Device.hpp>

namespace gui
{
    class Image;

    class AllDevicesWindow : public BaseSettingsWindow
    {
      public:
        explicit AllDevicesWindow(app::Application *app);

      private:
        void buildInterface() override;
        auto buildOptionsList() -> std::list<Option> override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        Image *leftArrowImage = nullptr;
        Image *crossImage     = nullptr;
        std::vector<Devicei> devices;
        std::string addressOfConnectedDevice;
        std::string addressOfSelectedDevice;
        std::unique_ptr<BluetoothSettingsModel> bluetoothSettingsModel;
    };

} // namespace gui
