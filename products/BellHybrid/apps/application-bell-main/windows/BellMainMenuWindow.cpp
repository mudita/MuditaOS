// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellMainMenuWindow.hpp"

#include <application-bell-alarm/ApplicationBellAlarm.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <application-bell-powernap/ApplicationBellPowerNap.hpp>
#include <data/BellMainStyle.hpp>

#include <Dialog.hpp>
#include <DialogMetadataMessage.hpp>
#include <OptionBellMenu.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{

    BellMainMenuWindow::BellMainMenuWindow(app::ApplicationCommon *app)
        : OptionWindow(app, gui::window::name::bell_main_menu)
    {
        addOptions(mainMenuOptionsList());
        buildInterface();
    }

    void BellMainMenuWindow::buildInterface()
    {
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        optionsList->setPosition(bellMainStyle::mainMenuWindow::options_list_x,
                                 bellMainStyle::mainMenuWindow::options_list_y);
        optionsList->setMaximumWidth(bellMainStyle::mainMenuWindow::default_body_width);
        optionsList->setBoundaries(gui::Boundaries::Fixed);
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

        addAppMenu(utils::translate("app_bellmain_alarm"), app::applicationBellAlarmName);
        addAppMenu(utils::translate("app_bellmain_power_nap"), app::applicationBellPowerNapName);
        // for demo only - to be replaced by call o final window
        addWinMenu(utils::translate("app_bellmain_meditation_timer"), gui::window::name::bell_main_menu_dialog);
        addWinMenu(utils::translate("app_bellmain_background_sounds"), gui::window::name::bell_main_menu_dialog);
        addWinMenu(utils::translate("app_bellmain_bedtime"), gui::window::name::bell_main_menu_dialog);

        addAppMenu(utils::translate("app_bellmain_settings"), app::applicationBellSettingsName);

        return menuOptionList;
    }

} // namespace gui
