// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimInfoWindow.hpp"
#include <locks/data/LockStyle.hpp>
#include <locks/data/LockData.hpp>
#include <i18n/i18n.hpp>

#include <application-settings/ApplicationSettings.hpp>

using namespace gui;

SimInfoWindow::SimInfoWindow(app::ApplicationCommon *app, const std::string &name) : WindowWithTimer(app, name)
{
    buildInterface();
}

status_bar::Configuration SimInfoWindow::configureStatusBar(status_bar::Configuration appConfiguration)
{
    appConfiguration.enable(status_bar::Indicator::NetworkAccessTechnology);
    appConfiguration.enable(status_bar::Indicator::Time);
    appConfiguration.enable(status_bar::Indicator::PhoneMode);
    appConfiguration.enable(status_bar::Indicator::Battery);
    appConfiguration.enable(status_bar::Indicator::Signal);
    appConfiguration.enable(status_bar::Indicator::SimCard);
    appConfiguration.enable(status_bar::Indicator::Bluetooth);
    appConfiguration.enable(status_bar::Indicator::AlarmClock);
    return appConfiguration;
}

void SimInfoWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    WindowWithTimer::onBeforeShow(mode, data);

    if (auto infoData = dynamic_cast<locks::SimLockData *>(data)) {
        switch (infoData->getSimInputTypeAction()) {
        case locks::SimInputTypeAction::UnlockWithPuk:
        case locks::SimInputTypeAction::ChangePin:
            setTitle(utils::translate("sim_change_pin"));
            infoIcon->text->setRichText(utils::translate("sim_pin_changed_successfully"));
            break;
        case locks::SimInputTypeAction::EnablePin:
            setTitle("");
            infoIcon->text->setRichText(utils::translate("sim_card_pin_enabled"));
            break;
        case locks::SimInputTypeAction::DisablePin:
            setTitle("");
            infoIcon->text->setRichText(utils::translate("sim_card_pin_disabled"));
            break;
        case locks::SimInputTypeAction::Error:
            setTitle(utils::translate("app_settings_net"));
            infoIcon->text->setRichText(utils::translate("sim_card_cant_connect"),
                                        {{"$SIM", infoData->getLock().getLockName()}});
            infoIcon->image->set("sim_128px_W_G");
            break;
        default:
            break;
        }
    }
}

void SimInfoWindow::buildInterface()
{
    AppWindow::buildInterface();

    setTitle(utils::translate("sim_change_pin"));

    infoIcon = new gui::Icon(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos,
                             style::window::default_body_width,
                             style::window::default_body_height,
                             "success_128px_W_G",
                             utils::translate("sim_pin_changed_successfully"));
    infoIcon->setAlignment(Alignment::Horizontal::Center);
}
