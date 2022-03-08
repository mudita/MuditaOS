// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimPINSettingsWindow.hpp"

#include <application-settings/data/PINSettingsLockStateData.hpp>
#include <application-settings/data/PINSettingsSimData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    SimPINSettingsWindow::SimPINSettingsWindow(app::ApplicationCommon *app)
        : BaseSettingsWindow(app, window::name::sim_pin_settings)
    {
        app->bus.sendUnicast<cellular::msg::request::sim::GetLockState>();
    }

    void SimPINSettingsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (const auto pinSettingsSimData = dynamic_cast<PINSettingsSimData *>(data); pinSettingsSimData != nullptr) {
            setTitle(utils::translate("app_settings_network_pin_settings") + " (" + pinSettingsSimData->getSim() + ")");
        }
        auto newPinLockStateRetrieved = false;
        if (const auto pinSettingsLockStateData = dynamic_cast<PINSettingsLockStateData *>(data);
            pinSettingsLockStateData != nullptr) {
            newPinLockStateRetrieved = pinIsOn != pinSettingsLockStateData->getSimCardPinLockState();
            if (newPinLockStateRetrieved) {
                pinIsOn = pinSettingsLockStateData->getSimCardPinLockState();
            }
        }
        if (mode == ShowMode::GUI_SHOW_RETURN) {
            application->bus.sendUnicast<cellular::msg::request::sim::GetLockState>();
        }
        if (mode == ShowMode::GUI_SHOW_INIT || newPinLockStateRetrieved) {
            refreshOptionsList();
        }
    }

    auto SimPINSettingsWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<Option> optionList;

        optionList.emplace_back(std::make_unique<option::OptionSettings>(
            utils::translate("app_settings_network_pin"),
            [=](Item & /*item*/) {
                changePinState(pinIsOn);
                return true;
            },
            [=](Item &item) {
                if (item.focus) {
                    this->setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
                }
                else {
                    this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
                }
                return true;
            },
            nullptr,
            pinIsOn ? option::SettingRightItem::On : option::SettingRightItem::Off));

        if (pinIsOn) {
            optionList.emplace_back(std::make_unique<option::OptionSettings>(
                utils::translate("app_settings_network_pin_change_code"),
                [=](Item & /*item*/) {
                    application->getSimLockSubject().changeSimPin();
                    return true;
                },
                nullptr,
                nullptr,
                option::SettingRightItem::ArrowWhite));
        }

        return optionList;
    }

    void SimPINSettingsWindow::changePinState(bool &currentState)
    {
        currentState = !currentState;
        if (!currentState) {
            application->getSimLockSubject().disableSimPin();
        }
        else {
            application->getSimLockSubject().enableSimPin();
        }
    }
} // namespace gui
