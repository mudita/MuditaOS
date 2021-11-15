// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsAlarmSettingsWindow.hpp"
#include "BellSettingsAlarmSettingsSnoozeWindow.hpp"
#include "BellSettingsAlarmSettingsMenuWindow.hpp"
#include "BellSettingsPrewakeUpWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <data/BellSettingsStyle.hpp>

namespace gui
{
    BellSettingsAlarmSettingsMenuWindow::BellSettingsAlarmSettingsMenuWindow(app::ApplicationCommon *app)
        : BellOptionWindow(app, name)
    {
        addOptions(alarmSettingsOptionsList());
        setListTitle(utils::translate("app_bell_settings_alarm_settings_title"));
    }

    std::list<Option> BellSettingsAlarmSettingsMenuWindow::alarmSettingsOptionsList()
    {
        std::list<gui::Option> alarmSettingsOptionsList;
        auto addAlarmSettingsOption = [&](const UTF8 &name, const std::string &window) {
            alarmSettingsOptionsList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
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
                [=](gui::Item &item) { return true; },
                this));
        };

        addAlarmSettingsOption(utils::translate("app_bell_settings_alarm_settings_prewake_up"),
                               BellSettingsPrewakeUpWindow::name);
        addAlarmSettingsOption(utils::translate("app_bell_settings_alarm_settings_alarm_tone_and_light"),
                               BellSettingsAlarmSettingsWindow::name);
        addAlarmSettingsOption(utils::translate("app_bell_settings_alarm_settings_snooze"),
                               BellSettingsAlarmSettingsSnoozeWindow::name);

        return alarmSettingsOptionsList;
    }
} /* namespace gui */
