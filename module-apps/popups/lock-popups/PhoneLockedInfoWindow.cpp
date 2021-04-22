// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockedInfoWindow.hpp"

#include "service-appmgr/Controller.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "RichTextParser.hpp"
#include "FontManager.hpp"

#include "application-desktop/data/DesktopStyle.hpp"
#include "module-apps/locks/data/LockStyle.hpp"
#include "module-apps/application-desktop/windows/Names.hpp"

#include <application-phonebook/ApplicationPhonebook.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

PhoneLockedInfoWindow::PhoneLockedInfoWindow(app::Application *app, const std::string &name) : AppWindow(app, name)
{
    buildInterface();
}

void PhoneLockedInfoWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setVisibleState();
}

void PhoneLockedInfoWindow::setVisibleState()
{
    lockImage->setVisible(true);

    bottomBar->setActive(BottomBar::Side::LEFT, true);
    bottomBar->setActive(BottomBar::Side::CENTER, false);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
}

bool PhoneLockedInfoWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortPress()) {
        if (inputEvent.keyCode == KeyCode::KEY_LF && bottomBar->isActive(BottomBar::Side::LEFT)) {
            app::manager::Controller::sendAction(application,
                                                 app::manager::actions::EmergencyDial,
                                                 std::make_unique<SwitchData>(),
                                                 app::manager::OnSwitchBehaviour::RunInBackground);
            return true;
        }
    }
    return AppWindow::onInput(inputEvent);
}

top_bar::Configuration PhoneLockedInfoWindow::configureTopBar(top_bar::Configuration appConfiguration)
{
    appConfiguration.disable(top_bar::Indicator::NetworkAccessTechnology);
    appConfiguration.disable(top_bar::Indicator::Time);
    appConfiguration.enable(top_bar::Indicator::PhoneMode);
    appConfiguration.enable(top_bar::Indicator::Lock);
    appConfiguration.enable(top_bar::Indicator::Battery);
    appConfiguration.enable(top_bar::Indicator::Signal);
    appConfiguration.enable(top_bar::Indicator::SimCard);
    return appConfiguration;
}

void PhoneLockedInfoWindow::buildInterface()
{
    namespace lock_style = style::window::pin_lock;
    AppWindow::buildInterface();

    bottomBar->setText(BottomBar::Side::LEFT, utils::translate("app_desktop_emergency"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::translate("common_back"));

    lockImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, "pin_lock");
    infoText  = new Text(this,
                        lock_style::primary_text::x,
                        lock_style::primary_text::y,
                        lock_style::primary_text::w,
                        lock_style::primary_text::h);

    infoText->setRichText(utils::translate("app_desktop_press_to_unlock"));
    infoText->setAlignment(Alignment::Horizontal::Center);
}
