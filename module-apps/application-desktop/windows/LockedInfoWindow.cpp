// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LockedInfoWindow.hpp"

#include "service-appmgr/Controller.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "RichTextParser.hpp"
#include "FontManager.hpp"

#include "application-desktop/data/DesktopStyle.hpp"
#include "locks/data/LockStyle.hpp"
#include "Names.hpp"

#include <application-phonebook/ApplicationPhonebook.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

LockedInfoWindow::LockedInfoWindow(app::Application *app) : AppWindow(app, app::window::name::desktop_locked)
{
    buildInterface();
}

void LockedInfoWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setVisibleState();
}

void LockedInfoWindow::setVisibleState()
{
    lockImage->setVisible(true);

    bottomBar->setActive(BottomBar::Side::LEFT, true);
    bottomBar->setActive(BottomBar::Side::CENTER, false);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
}

bool LockedInfoWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease()) {
        if (inputEvent.is(KeyCode::KEY_LF) && bottomBar->isActive(BottomBar::Side::LEFT)) {
            app::manager::Controller::sendAction(application, app::manager::actions::EmergencyDial);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_RF) && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            application->switchWindow(gui::name::window::main_window);
            return true;
        }
    }
    return AppWindow::onInput(inputEvent);
}

void LockedInfoWindow::rebuild()
{
    destroyInterface();
    buildInterface();
}

top_bar::Configuration LockedInfoWindow::configureTopBar(top_bar::Configuration appConfiguration)
{
    appConfiguration.enable(top_bar::Indicator::Lock);
    appConfiguration.disable(top_bar::Indicator::Time);
    return appConfiguration;
}

void LockedInfoWindow::buildInterface()
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

    TextFormat format(FontManager::getInstance().getFont(style::window::font::medium));
    text::RichTextParser rtParser;
    auto parsedText = rtParser.parse(utils::translate("app_desktop_press_to_unlock"), &format);

    infoText->setText(std::move(parsedText));
    infoText->setAlignment(Alignment::Horizontal::Center);
}

void LockedInfoWindow::destroyInterface()
{
    erase();
    invalidate();
}

void LockedInfoWindow::invalidate() noexcept
{
    lockImage = nullptr;
    infoText  = nullptr;
}
