// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellMainWindow.hpp"
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <service-appmgr/Controller.hpp>
#include <gui/input/InputEvent.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>

namespace gui
{
    BellMainWindow::BellMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
    {
        buildInterface();
    }

    void BellMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::start));
        setTitle(utils::translate("app_bellmain_main_window_title"));
    }

    bool BellMainWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_ENTER:
                LOG_INFO("Open MainMenu");
                application->switchWindow(gui::window::name::bell_main_menu, nullptr);
                return true;
            case KeyCode::KEY_RF:
                return true;
            default:
                break;
            }
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
