// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>
#include <application-settings/models/bluetooth/BluetoothSettingsModel.hpp>

namespace gui
{
    class Image;

    class AllDevicesWindow : public BaseSettingsWindow
    {
      public:
        AllDevicesWindow(app::ApplicationCommon *app, std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel);

      private:
        void buildInterface() override;
        auto buildOptionsList() -> std::list<Option> override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        UTF8 getTextOnCenter(const DeviceState &) const;
        UTF8 getTextOnRight(const DeviceState &) const;
        option::SettingRightItem getRightItem(const DeviceState &) const;
        auto handleDeviceAction(const Devicei &) -> bool;

        std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel{};
    };

} // namespace gui
