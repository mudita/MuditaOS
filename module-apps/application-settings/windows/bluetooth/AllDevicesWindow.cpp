// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AllDevicesWindow.hpp"

#include <application-settings/data/BondedDevicesData.hpp>
#include <application-settings/data/PairingDeviceData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <DialogMetadataMessage.hpp>
#include <InputEvent.hpp>
#include <OptionSetting.hpp>
#include <header/AddElementAction.hpp>

namespace gui
{

    AllDevicesWindow::AllDevicesWindow(app::Application *app) : BaseSettingsWindow(app, window::name::all_devices)
    {
        bluetoothSettingsModel = std::make_unique<BluetoothSettingsModel>(application);
        bluetoothSettingsModel->requestBondedDevices();
        buildInterface();
    }

    void AllDevicesWindow::buildInterface()
    {
        setTitle(utils::translate("app_settings_bluetooth_all_devices"));

        optionsList = new gui::ListView(this,
                                        option::window::optionsListX,
                                        option::window::optionsListY,
                                        option::window::optionsListW,
                                        option::window::optionsListH,
                                        optionsModel,
                                        listview::ScrollBarType::Proportional);

        optionsList->prepareRebuildCallback = [this]() { recreateOptions(); };
        optionsModel->createData(options);
        setFocusItem(optionsList);

        header->navigationIndicatorAdd(new gui::header::AddElementAction(), gui::header::BoxSelection::Left);
    }

    void AllDevicesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_RETURN) {
            bluetoothSettingsModel->stopScan();
            if (activeDevice.state == ActiveDevice::DeviceState::Connecting) {
                activeDevice.address.clear();
                activeDevice.state = ActiveDevice::DeviceState::Unknown;
            }
            else if (activeDevice.state == ActiveDevice::DeviceState::Pairing) {
                devices.erase(std::remove_if(devices.begin(),
                                             devices.end(),
                                             [&](const auto &device) {
                                                 return (bd_addr_to_str(device.address) == activeDevice.address);
                                             }),
                              devices.end());

                activeDevice.address.clear();
                activeDevice.state = ActiveDevice::DeviceState::Unknown;
            }
        }
        if (const auto bondedDevicesData = dynamic_cast<BondedDevicesData *>(data); bondedDevicesData != nullptr) {
            devices              = bondedDevicesData->getDevices();
            activeDevice.address = bondedDevicesData->getAddressOfConnectedDevice();
            activeDevice.state   = ActiveDevice::DeviceState::Connected;
        }
        else if (const auto pairingDeviceData = dynamic_cast<PairingDeviceData *>(data); pairingDeviceData != nullptr) {
            activeDevice.address = bd_addr_to_str(pairingDeviceData->getPairingDevice().address);
            activeDevice.state   = ActiveDevice::DeviceState::Pairing;
            devices.emplace_back(pairingDeviceData->getPairingDevice());
        }
        refreshOptionsList();
    }

    auto AllDevicesWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (!inputEvent.isShortRelease()) {
            return AppWindow::onInput(inputEvent);
        }
        if (inputEvent.is(KeyCode::KEY_LEFT)) {
            bluetoothSettingsModel->requestScan();
            application->switchWindow(gui::window::name::dialog_settings,
                                      gui::ShowMode::GUI_SHOW_INIT,
                                      std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
                                          utils::translate("app_settings_bluetooth_add_device"),
                                          "search_big",
                                          utils::translate("app_settings_bluetooth_searching_devices")}));
            return true;
        }
        if (inputEvent.is(KeyCode::KEY_LF)) {
            devices.erase(std::remove_if(devices.begin(),
                                         devices.end(),
                                         [&](const auto &device) {
                                             return (bd_addr_to_str(device.address) == addressOfDeviceSelected);
                                         }),
                          devices.end());
            bottomBar->setActive(BottomBar::Side::LEFT, false);
            bottomBar->setActive(BottomBar::Side::CENTER, false);
            bluetoothSettingsModel->requestDeviceUnpair(addressOfDeviceSelected);
            refreshOptionsList();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    auto AllDevicesWindow::buildOptionsList() -> std::list<Option>
    {
        bottomBar->setActive(BottomBar::Side::CENTER, !devices.empty());

        std::list<gui::Option> optionsList;
        for (const auto &device : devices) {
            ActiveDevice newDevice(bd_addr_to_str(device.address));
            newDevice.address == activeDevice.address ? newDevice.state = activeDevice.state
                                                      : newDevice.state = ActiveDevice::DeviceState::Paired;
            UTF8 textOnCenter                                           = getTextOnCenter(newDevice.state);
            option::SettingRightItem rightItem                          = getRightItem(newDevice.state);
            UTF8 textOnRight                                            = getTextOnRight(newDevice.state);

            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                device.name,
                [=](gui::Item & /*item*/) {
                    return handleDeviceAction(newDevice);
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(textOnCenter, BottomBar::Side::CENTER);
                        if (newDevice.state != ActiveDevice::DeviceState::Pairing) {
                            this->setBottomBarText(utils::translate("common_forget"), BottomBar::Side::LEFT);
                            this->bottomBar->setActive(BottomBar::Side::LEFT, true);
                        }
                        addressOfDeviceSelected = newDevice.address;
                    }
                    return true;
                },
                nullptr,
                rightItem,
                false,
                std::move(textOnRight)));
        }
        return optionsList;
    }

    UTF8 AllDevicesWindow::getTextOnCenter(const ActiveDevice::DeviceState &state) const
    {
        switch (state) {
        case ActiveDevice::DeviceState::Connected:
            return utils::translate("common_disconnect");
        case ActiveDevice::DeviceState::Paired:
            return utils::translate("common_connect");
        case ActiveDevice::DeviceState::Pairing:
        case ActiveDevice::DeviceState::Connecting:
        case ActiveDevice::DeviceState::Unknown:
            break;
        }
        return UTF8();
    }

    UTF8 AllDevicesWindow::getTextOnRight(const ActiveDevice::DeviceState &state) const
    {
        switch (state) {
        case ActiveDevice::DeviceState::Connected:
            return utils::translate("app_settings_option_connected");
        case ActiveDevice::DeviceState::Connecting:
            return utils::translate("app_settings_option_connecting");
        case ActiveDevice::DeviceState::Pairing:
            return utils::translate("app_settings_option_pairing");
        case ActiveDevice::DeviceState::Paired:
        case ActiveDevice::DeviceState::Unknown:
            break;
        }
        return UTF8();
    }

    option::SettingRightItem AllDevicesWindow::getRightItem(const ActiveDevice::DeviceState &state) const
    {
        switch (state) {
        case ActiveDevice::DeviceState::Connected:
        case ActiveDevice::DeviceState::Connecting:
        case ActiveDevice::DeviceState::Pairing:
            return option::SettingRightItem::Text;
        case ActiveDevice::DeviceState::Paired:
            return option::SettingRightItem::Bt;
        case ActiveDevice::DeviceState::Unknown:
            break;
        }
        return option::SettingRightItem::Disabled;
    }

    auto AllDevicesWindow::handleDeviceAction(const ActiveDevice &newDevice) -> bool
    {
        if (newDevice.state == ActiveDevice::DeviceState::Connected) {
            activeDevice.address.clear();
            activeDevice.state = ActiveDevice::DeviceState::Unknown;
            bluetoothSettingsModel->requestDisconnection();
            refreshOptionsList();
        }
        else if (newDevice.state == ActiveDevice::DeviceState::Paired) {
            activeDevice.address = newDevice.address;
            activeDevice.state   = ActiveDevice::DeviceState::Connecting;
            bluetoothSettingsModel->requestConnection(newDevice.address);
            refreshOptionsList();
        }
        return true;
    }

} // namespace gui
