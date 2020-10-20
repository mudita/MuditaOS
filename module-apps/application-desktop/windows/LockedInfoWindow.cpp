// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LockedInfoWindow.hpp"

#include "service-appmgr/ApplicationManager.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "RichTextParser.hpp"
#include "FontManager.hpp"

#include "application-desktop/data/AppDesktopStyle.hpp"
#include "Names.hpp"

#include <application-phonebook/ApplicationPhonebook.hpp>
#include <i18/i18.hpp>

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

    topBar->setActive(TopBar::Elements::LOCK, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);
    topBar->setActive(TopBar::Elements::SIGNAL, true);
}

bool LockedInfoWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortPress()) {
        if (inputEvent.keyCode == KeyCode::KEY_LF && bottomBar->isActive(BottomBar::Side::LEFT)) {
            sapm::ApplicationManager::messageSwitchApplication(
                application, app::name_phonebook, gui::window::name::ice_contacts, nullptr);
            return true;
        }
        else if (inputEvent.keyCode == KeyCode::KEY_RF && bottomBar->isActive(BottomBar::Side::RIGHT)) {
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

void LockedInfoWindow::buildInterface()
{
    AppWindow::buildInterface();

    bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_desktop_emergency"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    lockImage =
        new gui::Image(this, style::window::pin_lock::image_x, style::window::pin_lock::image_y, 0, 0, "pin_lock");

    infoText = new Text(this,
                        0,
                        style::window::pin_lock::info_text_y,
                        style::window_width,
                        style::window::pin_lock::info_text_h_screen);

    TextFormat format(FontManager::getInstance().getFont(style::window::font::medium));
    text::RichTextParser rtParser;
    auto parsedText = rtParser.parse(utils::localize.get("app_desktop_no_pin_lock"), &format);

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
