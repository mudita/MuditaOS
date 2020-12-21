// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"

#include "OptionSetting.hpp"

#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-bluetooth/messages/SetStatus.hpp>

namespace gui
{

    BluetoothWindow::BluetoothWindow(app::Application *app) : OptionWindow(app, gui::window::name::bluetooth)
    {
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::SIM, false);
        sys::Bus::SendUnicast(
            std::make_shared<message::bluetooth::RequestStatus>(), service::name::bluetooth, application);
    }

    void BluetoothWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data != nullptr) {
            const auto newData        = static_cast<BluetoothStatusData *>(data);
            isBluetoothSwitchOn       = newData->getState();
            isPhoneVisibilitySwitchOn = newData->getVisibility();
        }
        rebuildOptionList();
    }

    auto BluetoothWindow::bluetoothOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
            utils::translateI18("app_settings_bluetooth_main"),
            [=](gui::Item &item) {
                switchHandler(isBluetoothSwitchOn);
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translateI18(style::strings::common::Switch),
                                           BottomBar::Side::CENTER);
                }
                return true;
            },
            this,
            isBluetoothSwitchOn ? RightItem::On : RightItem::Off));

        if (isBluetoothSwitchOn) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                utils::translateI18("app_settings_bluetooth_all_devices_indent"),
                [=](gui::Item &item) {
                    this->application->switchWindow(gui::window::name::all_devices, gui::ShowMode::GUI_SHOW_INIT);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                nullptr,
                RightItem::ArrowWhite));
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                utils::translateI18("app_settings_bluetooth_phone_visibility"),
                [=](gui::Item &item) {
                    switchHandler(isPhoneVisibilitySwitchOn);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                isPhoneVisibilitySwitchOn ? RightItem::On : RightItem::Off));
            if (isPhoneVisibilitySwitchOn) {
                optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                    utils::translateI18("app_settings_bluetooth_phone_name_indent"),
                    [=](gui::Item &item) {
                        this->application->switchWindow(gui::window::name::phone_name, gui::ShowMode::GUI_SHOW_INIT);
                        return true;
                    },
                    [=](gui::Item &item) {
                        if (item.focus) {
                            this->setBottomBarText(utils::translateI18(style::strings::common::select),
                                                   BottomBar::Side::CENTER);
                        }
                        return true;
                    },
                    nullptr,
                    RightItem::ArrowWhite));
            }
        }

        return optionsList;
    }

    void BluetoothWindow::switchHandler(bool &switchState)
    {
        switchState = !switchState;
        BluetoothStatus btStatus;

        if (isBluetoothSwitchOn) {
            btStatus.state = BluetoothStatus::BluetoothState::On;
        }
        else {
            btStatus.state = BluetoothStatus::BluetoothState::Off;
        }
        btStatus.visibility = isPhoneVisibilitySwitchOn;
        message::bluetooth::SetStatus setStatus(btStatus);

        sys::Bus::SendUnicast(
            std::make_shared<message::bluetooth::SetStatus>(setStatus), service::name::bluetooth, application);
    }

    void BluetoothWindow::rebuildOptionList()
    {
        clearOptions();
        addOptions(bluetoothOptionsList());
    }

} // namespace gui
