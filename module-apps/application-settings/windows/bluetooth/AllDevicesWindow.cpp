﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AllDevicesWindow.hpp"

#include <application-settings/data/BondedDevicesData.hpp>
#include <application-settings/windows/WindowNames.hpp>
#include <application-settings/widgets/SettingsStyle.hpp>

#include <DialogMetadataMessage.hpp>
#include <InputEvent.hpp>
#include <OptionSetting.hpp>
#include <header/AddElementAction.hpp>

namespace gui
{

    AllDevicesWindow::AllDevicesWindow(app::ApplicationCommon *app,
                                       std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel)
        : BaseSettingsWindow(app, window::name::all_devices), bluetoothSettingsModel(bluetoothSettingsModel)
    {
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
        }
        if (const auto bondedDevicesData = dynamic_cast<BondedDevicesData *>(data); bondedDevicesData != nullptr) {
            bluetoothSettingsModel->replaceDevicesList(bondedDevicesData->getDevices());
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
                                          "search_128px_W_G",
                                          utils::translate("app_settings_bluetooth_searching_devices")}));
            return true;
        }
        if (inputEvent.is(KeyCode::KEY_LF) && !bluetoothSettingsModel->isDeviceListEmpty()) {
            navBar->setActive(nav_bar::Side::Left, false);
            navBar->setActive(nav_bar::Side::Center, false);
            auto selectedDevice = bluetoothSettingsModel->getSelectedDevice();
            if (selectedDevice.has_value()) {
                bluetoothSettingsModel->requestDeviceUnpair(selectedDevice.value().get());
            }
            refreshOptionsList();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    auto AllDevicesWindow::buildOptionsList() -> std::list<Option>
    {
        navBar->setActive(nav_bar::Side::Center, !bluetoothSettingsModel->getDevices().empty());
        std::list<gui::Option> optionsList;

        if (bluetoothSettingsModel->isDeviceListEmpty()) {
            this->navBar->setActive(nav_bar::Side::Left, false);
            return optionsList;
        }

        for (const auto &device : bluetoothSettingsModel->getDevices()) {
            UTF8 textOnCenter                  = getTextOnCenter(device.deviceState);
            option::SettingRightItem rightItem = getRightItem(device.deviceState);
            UTF8 textOnRight                   = getTextOnRight(device.deviceState);

            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                device.name.data(),
                [=](gui::Item & /*item*/) { return handleDeviceAction(device); },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setNavBarText(textOnCenter, nav_bar::Side::Center);
                        if (device.deviceState != DeviceState::Pairing) {
                            this->setNavBarText(utils::translate("common_forget"), nav_bar::Side::Left);
                            this->navBar->setActive(nav_bar::Side::Left, true);
                        }
                        bluetoothSettingsModel->setSelectedDevice(device);
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

    UTF8 AllDevicesWindow::getTextOnCenter(const DeviceState &state) const
    {
        switch (state) {
        case DeviceState::ConnectedAudio:
            [[fallthrough]];
        case DeviceState::ConnectedVoice:
            [[fallthrough]];
        case DeviceState::ConnectedBoth:
            return utils::translate("common_disconnect");
        case DeviceState::Paired:
            if (bluetoothSettingsModel->isDeviceConnecting()) {
                break;
            }
            return utils::translate("common_connect");
        case DeviceState::Pairing:
            [[fallthrough]];
        case DeviceState::Connecting:
            [[fallthrough]];
        case DeviceState::Unknown:
            break;
        }
        return UTF8();
    }

    UTF8 AllDevicesWindow::getTextOnRight(const DeviceState &state) const
    {
        switch (state) {
        case DeviceState::ConnectedBoth:
            return utils::translate("app_settings_option_connected_both");
        case DeviceState::Connecting:
            return utils::translate("app_settings_option_connecting");
        case DeviceState::Pairing:
            return utils::translate("app_settings_option_pairing");
        case DeviceState::ConnectedAudio:
            return utils::translate("app_settings_option_connected_audio");
        case DeviceState::ConnectedVoice:
            return utils::translate("app_settings_option_connected_voice");
        case DeviceState::Paired:
            [[fallthrough]];
        case DeviceState::Unknown:
            break;
        }
        return UTF8();
    }

    option::SettingRightItem AllDevicesWindow::getRightItem(const DeviceState &state) const
    {
        switch (state) {
        case DeviceState::ConnectedBoth:
            [[fallthrough]];
        case DeviceState::ConnectedAudio:
            [[fallthrough]];
        case DeviceState::ConnectedVoice:
            [[fallthrough]];
        case DeviceState::Connecting:
            [[fallthrough]];
        case DeviceState::Pairing:
            return option::SettingRightItem::Text;
        case DeviceState::Paired:
            return option::SettingRightItem::Bt;
        case DeviceState::Unknown:
            break;
        }
        return option::SettingRightItem::Disabled;
    }

    auto AllDevicesWindow::handleDeviceAction(const Devicei &device) -> bool
    {
        if (device.deviceState == DeviceState::ConnectedBoth || device.deviceState == DeviceState::ConnectedAudio ||
            device.deviceState == DeviceState::ConnectedVoice) {
            bluetoothSettingsModel->requestDisconnection();
            refreshOptionsList();
        }
        else if (device.deviceState == DeviceState::Paired && !bluetoothSettingsModel->isDeviceConnecting()) {
            bluetoothSettingsModel->requestConnection(device);
            refreshOptionsList();
        }
        return true;
    }

} // namespace gui
