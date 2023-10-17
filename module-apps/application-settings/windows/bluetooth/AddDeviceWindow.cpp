// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AddDeviceWindow.hpp"

#include <application-settings/data/DeviceData.hpp>
#include <application-settings/data/PairingDeviceData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{

    AddDeviceWindow::AddDeviceWindow(app::ApplicationCommon *app,
                                     std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel)
        : BaseSettingsWindow(app, window::name::add_device), bluetoothSettingsModel(bluetoothSettingsModel)
    {
        setTitle(utils::translate("app_settings_bluetooth_add_device"));
    }

    void AddDeviceWindow::onBeforeShow(ShowMode /*mode*/, SwitchData *data)
    {
        const auto newData = dynamic_cast<DeviceData *>(data);
        if (newData != nullptr) {
            devices = newData->getDevices();
        }
        refreshOptionsList();
    }

    void AddDeviceWindow::onClose([[maybe_unused]] CloseReason reason)
    {
        bluetoothSettingsModel->stopScan();
    }

    auto AddDeviceWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        for (auto &device : devices) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                device.name.data(),
                [&](gui::Item & /*unused*/) {
                    bluetoothSettingsModel->requestDevicePair(device);
                    application->switchWindow(gui::window::name::all_devices);
                    return true;
                },
                nullptr,
                nullptr,
                gui::option::SettingRightItem::Bt));
        }

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::add));

        return optionsList;
    }
} // namespace gui
