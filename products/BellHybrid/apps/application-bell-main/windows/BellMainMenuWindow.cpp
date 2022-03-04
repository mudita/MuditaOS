// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellMainMenuWindow.hpp"

#include <application-bell-alarm/ApplicationBellAlarm.hpp>
#include <application-bell-background-sounds/ApplicationBellBackgroundSounds.hpp>
#include <application-bell-bedtime/ApplicationBellBedtime.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <application-bell-meditation-timer/ApplicationBellMeditationTimer.hpp>
#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <application-bell-powernap/ApplicationBellPowerNap.hpp>

#include <Dialog.hpp>
#include <DialogMetadataMessage.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    BellMainMenuWindow::BellMainMenuWindow(app::ApplicationCommon *app)
        : BellOptionWindow(app, gui::window::name::bell_main_menu)
    {
        addOptions(mainMenuOptionsList());
    }

    std::list<Option> BellMainMenuWindow::mainMenuOptionsList()
    {
        std::list<gui::Option> menuOptionList;

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
        addAppMenu(utils::translate("app_bellmain_background_sounds"), app::applicationBellBackgroundSoundsName);
        addAppMenu(utils::translate("app_bellmain_meditation_timer"), app::applicationBellMeditationTimerName);
        addAppMenu(utils::translate("app_bellmain_bedtime"), app::applicationBellBedtimeName);
        addAppMenu(utils::translate("app_bellmain_settings"), app::applicationBellSettingsName);

        return menuOptionList;
    }

} // namespace gui
