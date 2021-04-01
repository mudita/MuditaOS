// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PINSettingsWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/data/PINSettingsLockStateData.hpp"
#include "application-settings-new/data/PINSettingsSimData.hpp"
#include "OptionSetting.hpp"

#include <service-appmgr/Controller.hpp>
#include "service-cellular/CellularServiceAPI.hpp"

namespace gui
{
    PINSettingsWindow::PINSettingsWindow(app::Application *app) : BaseSettingsWindow(app, window::name::pin_settings)
    {
        CellularServiceAPI::RequestSimCardPinLockState(app);
    }

    void PINSettingsWindow::onBeforeShow(ShowMode /*mode*/, SwitchData *data)
    {
        if (const auto pinSettingsSimData = dynamic_cast<PINSettingsSimData *>(data); pinSettingsSimData != nullptr) {
            setTitle(utils::translateI18("app_settings_network_pin_settings") + " (" + pinSettingsSimData->getSim() +
                     ")");
        }
        if (const auto pinSettingsLockStateData = dynamic_cast<PINSettingsLockStateData *>(data);
            pinSettingsLockStateData != nullptr) {
            pinIsOn = pinSettingsLockStateData->getSimCardPinLockState();
        }
        refreshOptionsList();
    }

    auto PINSettingsWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<Option> optionList;

        optionList.emplace_back(std::make_unique<option::OptionSettings>(
            utils::translateI18("app_settings_network_pin"),
            [=](Item & /*item*/) {
                changePinState(pinIsOn);
                return true;
            },
            [=](Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::localize.get(style::strings::common::Switch),
                                           BottomBar::Side::CENTER);
                }
                else {
                    this->setBottomBarText(utils::localize.get(style::strings::common::select),
                                           BottomBar::Side::CENTER);
                }
                return true;
            },
            nullptr,
            pinIsOn ? option::SettingRightItem::On : option::SettingRightItem::Off));

        if (pinIsOn) {
            optionList.emplace_back(std::make_unique<option::OptionSettings>(
                utils::translateI18("app_settings_network_pin_change_code"),
                [=](Item & /*item*/) {
                    using namespace app::manager::actions;
                    auto params = std::make_unique<PasscodeParams>(Store::GSM::get()->selected,
                                                                   PasscodeParams::numOfAttemptsForEnteringPIN,
                                                                   PasscodeParams::pinName);
                    app::manager::Controller::sendAction(application, RequestPinChange, std::move(params));
                    return true;
                },
                nullptr,
                nullptr,
                option::SettingRightItem::ArrowWhite));
        }

        return optionList;
    }

    void PINSettingsWindow::changePinState(bool &currentState)
    {
        currentState = !currentState;
        refreshOptionsList();
        if (!currentState) {
            app::manager::Controller::sendAction(application, app::manager::actions::RequestPinDisable, nullptr);
        }
        else {
            app::manager::Controller::sendAction(application, app::manager::actions::RequestPinEnable, nullptr);
        }
    }
} // namespace gui
