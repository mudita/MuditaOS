// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimNotReadyWindow.hpp"

#include <service-appmgr/Controller.hpp>
#include <application-desktop/data/DesktopStyle.hpp>

#include <i18n/i18n.hpp>

using namespace gui;

SimNotReadyWindow::SimNotReadyWindow(app::Application *app, const std::string &name) : AppWindow(app, name)
{
    buildInterface();
}

top_bar::Configuration SimNotReadyWindow::configureTopBar(top_bar::Configuration appConfiguration)
{
    appConfiguration.disable(top_bar::Indicator::NetworkAccessTechnology);
    appConfiguration.disable(top_bar::Indicator::Lock);
    appConfiguration.enable(top_bar::Indicator::PhoneMode);
    appConfiguration.enable(top_bar::Indicator::Time);
    appConfiguration.enable(top_bar::Indicator::Battery);
    appConfiguration.enable(top_bar::Indicator::Signal);
    appConfiguration.enable(top_bar::Indicator::SimCard);
    return appConfiguration;
}

void SimNotReadyWindow::buildInterface()
{
    AppWindow::buildInterface();

    setTitle(utils::translate("app_settings_net"));

    bottomBar->setText(BottomBar::Side::RIGHT, utils::translate("common_back"));

    infoIcon = new gui::Icon(this,
                             style::window::default_left_margin,
                             style::header::height,
                             style::window::default_body_width,
                             style::window::default_body_height,
                             "sim_card_W_G",
                             utils::translate("sim_card_not_ready"));
    infoIcon->setAlignment(Alignment::Horizontal::Center);
}
