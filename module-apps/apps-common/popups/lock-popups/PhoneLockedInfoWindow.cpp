// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockedInfoWindow.hpp"

#include <service-appmgr/Controller.hpp>
#include <application-desktop/data/DesktopStyle.hpp>

#include <i18n/i18n.hpp>

using namespace gui;

PhoneLockedInfoWindow::PhoneLockedInfoWindow(app::Application *app, const std::string &name) : AppWindow(app, name)
{
    buildInterface();
}

void PhoneLockedInfoWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    bottomBar->setActive(BottomBar::Side::LEFT, true);
    bottomBar->setActive(BottomBar::Side::CENTER, false);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
}

bool PhoneLockedInfoWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease(KeyCode::KEY_LF) && bottomBar->isActive(BottomBar::Side::LEFT)) {
        app::manager::Controller::sendAction(application,
                                             app::manager::actions::EmergencyDial,
                                             std::make_unique<SwitchData>(),
                                             app::manager::OnSwitchBehaviour::RunInBackground);
        return true;
    }
    return AppWindow::onInput(inputEvent);
}

status_bar::Configuration PhoneLockedInfoWindow::configureStatusBar(status_bar::Configuration appConfiguration)
{
    appConfiguration.disable(status_bar::Indicator::NetworkAccessTechnology);
    appConfiguration.disable(status_bar::Indicator::Time);
    appConfiguration.enable(status_bar::Indicator::PhoneMode);
    appConfiguration.enable(status_bar::Indicator::Lock);
    appConfiguration.enable(status_bar::Indicator::Battery);
    appConfiguration.enable(status_bar::Indicator::Signal);
    appConfiguration.enable(status_bar::Indicator::SimCard);
    return appConfiguration;
}

void PhoneLockedInfoWindow::buildInterface()
{
    AppWindow::buildInterface();

    bottomBar->setText(BottomBar::Side::LEFT, utils::translate("app_desktop_emergency"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::translate("common_back"));

    infoIcon = new gui::Icon(this,
                             style::window::default_left_margin,
                             style::header::height,
                             style::window::default_body_width,
                             style::window::default_body_height,
                             "unlock_icon_W_G",
                             utils::translate("app_desktop_press_to_unlock"));
    infoIcon->setAlignment(Alignment::Horizontal::Center);
}
