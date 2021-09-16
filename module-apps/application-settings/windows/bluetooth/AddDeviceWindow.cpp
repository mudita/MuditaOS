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
                device.name,
                [&](gui::Item & /*unused*/) {
                    bluetoothSettingsModel->requestDevicePair(device);
                    application->switchWindow(gui::window::name::all_devices);
                    return true;
                },
                nullptr,
                nullptr,
                gui::option::SettingRightItem::Bt));
        }

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::add));

        return optionsList;
    }
} // namespace gui
