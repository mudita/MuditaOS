// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"

#include "OptionSetting.hpp"

#include <i18/i18.hpp>
#include <module-services/service-bluetooth/messages/BluetoothMessage.hpp>

namespace gui
{

    BluetoothWindow::BluetoothWindow(app::Application *app) : OptionWindow(app, gui::window::name::bluetooth)
    {}

    void BluetoothWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        rebuildOptionList();
    }

    auto BluetoothWindow::bluetoothOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
            utils::translateI18("app_settings_bluetooth_main"),
            [=](gui::Item &item) {
                bluetoothSwitchHandler(isBluetoothSwitchOn);
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::localize.get(style::strings::common::Switch),
                                           BottomBar::Side::CENTER);
                }
                return true;
            },
            this));
        if (isBluetoothSwitchOn) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                utils::translateI18("app_settings_bluetooth_all_devices_indent"),
                [=](gui::Item &item) {
                    this->application->switchWindow(gui::window::name::all_devices, gui::ShowMode::GUI_SHOW_INIT);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::localize.get(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                nullptr,
                RightIcon::Border));
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                utils::translateI18("app_settings_bluetooth_phone_visibility"),
                [=](gui::Item &item) {
                    phoneVisibilitySwitchHandler(isPhoneVisibilitySwitchOn);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::localize.get(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this));
            if (isPhoneVisibilitySwitchOn) {
                optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                    utils::translateI18("app_settings_bluetooth_phone_name_indent"),
                    [=](gui::Item &item) {
                        this->application->switchWindow(gui::window::name::phone_name, gui::ShowMode::GUI_SHOW_INIT);
                        return true;
                    },
                    [=](gui::Item &item) {
                        if (item.focus) {
                            this->setBottomBarText(utils::localize.get(style::strings::common::select),
                                                   BottomBar::Side::CENTER);
                        }
                        return true;
                    },
                    nullptr,
                    RightIcon::Border));
            }
        }

        return optionsList;
    }

    void BluetoothWindow::bluetoothSwitchHandler(bool &switchState)
    {
        if (switchState) {
            sys::Bus::SendUnicast(
                std::make_shared<BluetoothMessage>(BluetoothMessage::Request::Stop), "ServiceBluetooth", application);
        }
        else {
            sys::Bus::SendUnicast(
                std::make_shared<BluetoothMessage>(BluetoothMessage::Request::Start), "ServiceBluetooth", application);
        }
        switchState = !switchState;
        rebuildOptionList();
    }

    void BluetoothWindow::phoneVisibilitySwitchHandler(bool &switchState)
    {
        sys::Bus::SendUnicast(
            std::make_shared<BluetoothMessage>(BluetoothMessage::Request::Visible), "ServiceBluetooth", application);

        switchState = !switchState;
        rebuildOptionList();
    }

    void BluetoothWindow::rebuildOptionList()
    {
        clearOptions();
        addOptions(bluetoothOptionsList());
    }

} // namespace gui
