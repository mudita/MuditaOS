﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AllDevicesWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/data/BondedDevicesData.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"

#include "DialogMetadataMessage.hpp"
#include "OptionSetting.hpp"

#include <InputEvent.hpp>

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
        setTitle(utils::localize.get("app_settings_bluetooth_all_devices"));
        leftArrowImage = new gui::Image(this,
                                        style::settings::window::leftArrowImage::x,
                                        style::settings::window::leftArrowImage::y,
                                        style::settings::window::leftArrowImage::w,
                                        style::settings::window::leftArrowImage::h,
                                        "arrow_left");
        crossImage     = new gui::Image(this,
                                    style::settings::window::crossImage::x,
                                    style::settings::window::crossImage::y,
                                    style::settings::window::crossImage::w,
                                    style::settings::window::crossImage::h,
                                    "cross");
    }

    void AllDevicesWindow::onBeforeShow(ShowMode /*mode*/, SwitchData *data)
    {
        const auto newData = dynamic_cast<BondedDevicesData *>(data);
        if (newData != nullptr) {
            devices = newData->getDevices();
        }
        refreshOptionsList();
    }

    auto AllDevicesWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortPress() && inputEvent.is(KeyCode::KEY_LEFT)) {
            bluetoothSettingsModel->requestScan();
            gui::DialogMetadata meta;
            application->switchWindow(gui::window::name::dialog_settings,
                                      gui::ShowMode::GUI_SHOW_INIT,
                                      std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
                                          utils::localize.get("app_settings_bluetooth_add_device"),
                                          "search_big",
                                          utils::localize.get("app_settings_bluetooth_searching_devices")}));
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    auto AllDevicesWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<gui::Option> optionsList;
        for (const auto &device : devices) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                device.name,
                [=](gui::Item & /*item*/) {
                    LOG_DEBUG("Device: %s", device.name.c_str());
                    return true;
                },
                nullptr,
                nullptr,
                gui::option::SettingRightItem::Bt));
        }
        return optionsList;
    }
} // namespace gui
