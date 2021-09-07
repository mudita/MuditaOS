// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsAlarmSettingsWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/options/type/OptionBellMenu.hpp>
#include <data/BellSettingsStyle.hpp>

namespace gui
{
    BellSettingsAlarmSettingsWindow::BellSettingsAlarmSettingsWindow(app::Application *app, std::string name)
        : OptionWindow(app, std::move(name))
    {
        addOptions(alarmSettingsOptionsList());
        buildInterface();
    }

    void BellSettingsAlarmSettingsWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsAlarmSettingsWindow::buildInterface()
    {
        header->setTitle(title);
        header->setEdges(RectangleEdge::None);
        statusBar->setVisible(false);
        bottomBar->setVisible(false);

        optionsList->setPosition(bell_settings_style::alarm_settings_window::options_list_margin_x,
                                 bell_settings_style::alarm_settings_window::options_list_margin_y);
        optionsList->setMaximumWidth(bell_settings_style::alarm_settings_window::default_body_width);
        optionsList->setBoundaries(gui::Boundaries::Fixed);
    }

    std::list<Option> BellSettingsAlarmSettingsWindow::alarmSettingsOptionsList()
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
                [=](gui::Item &item) {
                    // put focus change callback here
                    return true;
                },
                this));
        };

        addAlarmSettingsOption(utils::translate("app_bell_settings_alarm_settings_prewake_up"),
                               gui::window::name::bellSettingsAlarmSettingsPrewakeUp);
        addAlarmSettingsOption(utils::translate("app_bell_settings_alarm_settings_alarm_tone_and_light"),
                               gui::window::name::bellSettingsAlarmSettingsAlarmToneAndLight);
        addAlarmSettingsOption(utils::translate("app_bell_settings_alarm_settings_snooze"),
                               gui::window::name::bellSettingsAlarmSettingsSnooze);

        return alarmSettingsOptionsList;
    }
} /* namespace gui */
