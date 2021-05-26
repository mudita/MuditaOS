// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimInfoWindow.hpp"
#include <locks/data/LockStyle.hpp>
#include <locks/data/LockData.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

SimInfoWindow::SimInfoWindow(app::Application *app, const std::string &name) : WindowWithTimer(app, name)
{
    buildInterface();
}

top_bar::Configuration SimInfoWindow::configureTopBar(top_bar::Configuration appConfiguration)
{
    appConfiguration.enable(top_bar::Indicator::NetworkAccessTechnology);
    appConfiguration.enable(top_bar::Indicator::Time);
    appConfiguration.enable(top_bar::Indicator::PhoneMode);
    appConfiguration.enable(top_bar::Indicator::Battery);
    appConfiguration.enable(top_bar::Indicator::Signal);
    appConfiguration.enable(top_bar::Indicator::SimCard);
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
                             style::header::height,
                             style::window::default_body_width,
                             style::window::default_body_height,
                             "success_icon_W_G",
                             utils::translate("sim_pin_changed_successfully"));
    infoIcon->setAlignment(Alignment::Horizontal::Center);
}
