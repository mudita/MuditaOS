// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothWindow.hpp"

#include <application-settings/data/BluetoothStatusData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{

    BluetoothWindow::BluetoothWindow(app::ApplicationCommon *app,
                                     std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel)
        : BaseSettingsWindow(app, window::name::bluetooth), bluetoothSettingsModel(std::move(bluetoothSettingsModel))
    {
        setTitle(utils::translate("app_settings_bluetooth_main"));
    }

    void BluetoothWindow::onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        const auto bluetoothStatus = bluetoothSettingsModel->getStatus();

        isBluetoothSwitchOn       = bluetoothStatus.state == BluetoothStatus::State::On;
        isPhoneVisibilitySwitchOn = bluetoothStatus.visibility;

        refreshOptionsList();
    }

    auto BluetoothWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_bluetooth_main"),
            [=](gui::Item & /*unused*/) {
                changeBluetoothState(isBluetoothSwitchOn);
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
                }
                return true;
            },
            this,
            isBluetoothSwitchOn ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));

        if (isBluetoothSwitchOn) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translate("app_settings_bluetooth_all_devices"),
                [=](gui::Item & /*unused*/) {
                    this->application->switchWindow(gui::window::name::all_devices, gui::ShowMode::GUI_SHOW_INIT);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
                    }
                    return true;
                },
                nullptr,
                gui::option::SettingRightItem::ArrowWhite,
                true));
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translate("app_settings_bluetooth_phone_visibility"),
                [=](gui::Item & /*unused*/) {
                    changeVisibility(isPhoneVisibilitySwitchOn);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
                    }
                    return true;
                },
                this,
                isPhoneVisibilitySwitchOn ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));
            if (isPhoneVisibilitySwitchOn) {
                optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                    utils::translate("app_settings_bluetooth_phone_name"),
                    [=](gui::Item & /*unused*/) {
                        this->application->switchWindow(gui::window::name::phone_name, gui::ShowMode::GUI_SHOW_INIT);
                        return true;
                    },
                    [=](gui::Item &item) {
                        if (item.focus) {
                            this->setNavBarText(utils::translate(style::strings::common::select),
                                                nav_bar::Side::Center);
                        }
                        return true;
                    },
                    nullptr,
                    gui::option::SettingRightItem::ArrowWhite,
                    true));
            }
        }

        return optionsList;
    }

    void BluetoothWindow::changeBluetoothState(bool &currentState)
    {
        currentState = !currentState;
        refreshOptionsList();
        bluetoothSettingsModel->setStatus(currentState, isPhoneVisibilitySwitchOn);
    }

    void BluetoothWindow::changeVisibility(bool &currentVisibility)
    {
        currentVisibility = !currentVisibility;
        refreshOptionsList();
        bluetoothSettingsModel->setStatus(isBluetoothSwitchOn, currentVisibility);
    }
} // namespace gui
