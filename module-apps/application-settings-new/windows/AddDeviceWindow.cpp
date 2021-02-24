// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AddDeviceWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/data/DeviceData.hpp"

#include "OptionSetting.hpp"

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
}

namespace gui
{

    AddDeviceWindow::AddDeviceWindow(app::Application *app) : BaseSettingsWindow(app, window::name::add_device)
    {
        bluetoothSettingsModel = std::make_unique<BluetoothSettingsModel>(application);
    }

    void AddDeviceWindow::onBeforeShow(ShowMode /*mode*/, SwitchData *data)
    {
        const auto newData = dynamic_cast<DeviceData *>(data);
        if (newData != nullptr) {
            devices = newData->getDevices();
        }
        refreshOptionsList();
    }

    void AddDeviceWindow::onClose()
    {
        bluetoothSettingsModel->stopScan();
    }

    auto AddDeviceWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        for (const auto &device : devices) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                device.name,
                [=](gui::Item & /*unused*/) {
                    LOG_DEBUG("Device: %s", device.name.c_str());
                    bluetoothSettingsModel->setAddrForAudioProfiles(bd_addr_to_str(device.address));
                    bluetoothSettingsModel->requestDevicePairing(bd_addr_to_str(device.address));
                    return true;
                },
                nullptr,
                nullptr,
                gui::option::SettingRightItem::Bt));
        }

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::add));

        return optionsList;
    }
} // namespace gui
