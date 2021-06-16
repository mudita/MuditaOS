// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellMainMenuWindow.hpp"
#include <OptionBellMenu.hpp>
#include <service-appmgr/Controller.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <windows/Dialog.hpp>
#include <DialogMetadataMessage.hpp>
#include <application-bell-main/data/BellMainStyle.hpp>

namespace gui
{

    BellMainMenuWindow::BellMainMenuWindow(app::Application *app) : OptionWindow(app, gui::window::name::bell_main_menu)
    {
        addOptions(mainMenuOptionsList());
        buildInterface();
    }

    void BellMainMenuWindow::buildInterface()
    {
        bottomBar->setVisible(false);
        statusBar->setVisible(false);
        title->visible = false;
        optionsList->setPosition(bellMainStyle::mainMenuWindow::options_list_x,
                                 bellMainStyle::mainMenuWindow::options_list_y);
        optionsList->setMaximumWidth(bellMainStyle::mainMenuWindow::default_body_width);
        optionsList->setBoundaries(listview::Boundaries::Continuous);
    }

    std::list<Option> BellMainMenuWindow::mainMenuOptionsList()
    {
        std::list<gui::Option> menuOptionList;
        auto addWinMenu = [&](const UTF8 &name, const std::string &window) {
            menuOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                name,
                [=](gui::Item &item) {
                    if (window.empty()) {
                        return false;
                    }
                    application->switchWindow(
                        window,
                        gui::ShowMode::GUI_SHOW_INIT,
                        std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{name, "search_big", " "}));
                    return true;
                },
                [=](gui::Item &item) {
                    // put focus change callback here
                    return true;
                },
                this));
        };

        auto addAppMenu = [&](const UTF8 &name, const std::string &appName) {
            menuOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                name,
                [=](gui::Item &item) {
                    if (appName.empty()) {
                        return false;
                    }
                    return app::manager::Controller::sendAction(application,
                                                                app::manager::actions::Launch,
                                                                std::make_unique<app::ApplicationLaunchData>(appName));
                },
                [=](gui::Item &item) {
                    // put focus change callback here
                    return true;
                },
                this));
        };

        addAppMenu(utils::translate("app_bellmain_next_alarm"), "");

        // for demo only - to be replaced by call o final window
        addWinMenu(utils::translate("app_bellmain_power_nap"), gui::window::name::bell_main_menu_dialog);
        addWinMenu(utils::translate("app_bellmain_meditation_timer"), gui::window::name::bell_main_menu_dialog);
        addWinMenu(utils::translate("app_bellmain_media_library"), gui::window::name::bell_main_menu_dialog);
        addWinMenu(utils::translate("app_bellmain_settings"), gui::window::name::bell_main_menu_dialog);

        return menuOptionList;
    }

} // namespace gui
