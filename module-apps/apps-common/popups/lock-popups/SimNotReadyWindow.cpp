// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimNotReadyWindow.hpp"

#include <service-appmgr/Controller.hpp>

#include <i18n/i18n.hpp>

using namespace gui;

SimNotReadyWindow::SimNotReadyWindow(app::ApplicationCommon *app, const std::string &name) : AppWindow(app, name)
{
    buildInterface();
}

status_bar::Configuration SimNotReadyWindow::configureStatusBar(status_bar::Configuration appConfiguration)
{
    appConfiguration.disable(status_bar::Indicator::NetworkAccessTechnology);
    appConfiguration.disable(status_bar::Indicator::Lock);
    appConfiguration.enable(status_bar::Indicator::PhoneMode);
    appConfiguration.enable(status_bar::Indicator::Time);
    appConfiguration.enable(status_bar::Indicator::Battery);
    appConfiguration.enable(status_bar::Indicator::Signal);
    appConfiguration.enable(status_bar::Indicator::SimCard);
    appConfiguration.enable(status_bar::Indicator::Bluetooth);
    appConfiguration.enable(status_bar::Indicator::AlarmClock);
    return appConfiguration;
}

void SimNotReadyWindow::buildInterface()
{
    AppWindow::buildInterface();

    setTitle(utils::translate("app_settings_net"));

    bottomBar->setText(BottomBar::Side::RIGHT, utils::translate("common_back"));

    infoIcon = new gui::Icon(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos,
                             style::window::default_body_width,
                             style::window::default_body_height,
                             "sim_card_W_G",
                             utils::translate("sim_card_not_ready"));
    infoIcon->setAlignment(Alignment::Horizontal::Center);
}
