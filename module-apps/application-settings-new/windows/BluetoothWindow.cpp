// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        sys::Bus::SendUnicast(
            std::make_shared<::message::bluetooth::RequestStatus>(), service::name::bluetooth, application);
    }

    void BluetoothWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data != nullptr) {
            const auto newData = static_cast<BluetoothStatusData *>(data);
            if (const auto btState = newData->getState(); btState.has_value()) {
                isBluetoothSwitchOn = btState.value();
            }
            if (const auto visibility = newData->getVisibility(); visibility.has_value()) {
                isPhoneVisibilitySwitchOn = visibility.value();
            }
        }
        rebuildOptionList();
    }

    auto BluetoothWindow::bluetoothOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translateI18("app_settings_bluetooth_main"),
            [=](gui::Item &item) {
                changeBluetoothState(isBluetoothSwitchOn);
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
            isBluetoothSwitchOn ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));

        if (isBluetoothSwitchOn) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translateI18("app_settings_bluetooth_all_devices"),
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
                gui::option::SettingRightItem::ArrowWhite,
                true));
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translateI18("app_settings_bluetooth_phone_visibility"),
                [=](gui::Item &item) {
                    changeVisibility(isPhoneVisibilitySwitchOn);
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
                isPhoneVisibilitySwitchOn ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));
            if (isPhoneVisibilitySwitchOn) {
                optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                    utils::translateI18("app_settings_bluetooth_phone_name"),
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
                    gui::option::SettingRightItem::ArrowWhite,
                    true));
            }
        }

        return optionsList;
    }

    void BluetoothWindow::changeBluetoothState(bool currentState)
    {
        ::message::bluetooth::SetStatus setStatus(makeDesiredStatus(!currentState, isPhoneVisibilitySwitchOn));
        sys::Bus::SendUnicast(std::make_shared<::message::bluetooth::SetStatus>(std::move(setStatus)),
                              service::name::bluetooth,
                              application);
    }

    void BluetoothWindow::changeVisibility(bool currentVisibility)
    {
        ::message::bluetooth::SetStatus setStatus(makeDesiredStatus(isBluetoothSwitchOn, !currentVisibility));
        sys::Bus::SendUnicast(std::make_shared<::message::bluetooth::SetStatus>(std::move(setStatus)),
                              service::name::bluetooth,
                              application);
    }

    BluetoothStatus BluetoothWindow::makeDesiredStatus(bool desiredBluetoothState, bool desiredVisibility) noexcept
    {
        BluetoothStatus status;
        status.state      = desiredBluetoothState ? BluetoothStatus::State::On : BluetoothStatus::State::Off;
        status.visibility = desiredVisibility;
        return status;
    }

    void BluetoothWindow::rebuildOptionList()
    {
        clearOptions();
        addOptions(bluetoothOptionsList());
    }

} // namespace gui
