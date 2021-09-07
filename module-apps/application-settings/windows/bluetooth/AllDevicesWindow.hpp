// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <application-settings/models/bluetooth/BluetoothSettingsModel.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

#include <Device.hpp>

namespace gui
{
    class Image;

    class ActiveDevice
    {
      public:
        explicit ActiveDevice(std::string address) : address(std::move(address))
        {}
        ActiveDevice() = default;
        enum class DeviceState
        {
            Connected,
            Connecting,
            Pairing,
            Paired,
            Unknown
        };
        DeviceState state = DeviceState::Unknown;
        std::string address;
    };

    class AllDevicesWindow : public BaseSettingsWindow
    {
      public:
        explicit AllDevicesWindow(app::Application *app);

      private:
        void buildInterface() override;
        auto buildOptionsList() -> std::list<Option> override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        UTF8 getTextOnCenter(const ActiveDevice::DeviceState &) const;
        UTF8 getTextOnRight(const ActiveDevice::DeviceState &) const;
        option::SettingRightItem getRightItem(const ActiveDevice::DeviceState &) const;
        auto handleDeviceAction(const ActiveDevice &) -> bool;

        ActiveDevice activeDevice;
        std::vector<Devicei> devices{};
        std::string addressOfDeviceSelected;
        std::unique_ptr<BluetoothSettingsModel> bluetoothSettingsModel{};
    };

} // namespace gui
