// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothWindow.hpp"

#include <application-settings/data/BluetoothStatusData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{

    BluetoothWindow::BluetoothWindow(app::ApplicationCommon *app) : BaseSettingsWindow(app, window::name::bluetooth)
    {
        setTitle(utils::translate("app_settings_bluetooth_main"));

        bluetoothSettingsModel = std::make_unique<BluetoothSettingsModel>(application);
        bluetoothSettingsModel->requestStatus();
    }

    void BluetoothWindow::onBeforeShow(ShowMode /*mode*/, SwitchData *data)
    {
        const auto newData = dynamic_cast<BluetoothStatusData *>(data);
        if (newData != nullptr) {
            if (const auto btState = newData->getState(); btState.has_value()) {
                isBluetoothSwitchOn = btState.value();
            }
            if (const auto visibility = newData->getVisibility(); visibility.has_value()) {
                isPhoneVisibilitySwitchOn = visibility.value();
            }
        }
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
