// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsWindow.hpp"
#include "alarm_settings/BellSettingsAlarmSettingsWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <data/BellSettingsStyle.hpp>

#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/options/type/OptionBellMenu.hpp>
#include <apps-common/windows/Dialog.hpp>

namespace gui
{

    BellSettingsWindow::BellSettingsWindow(app::Application *app) : OptionWindow(app, gui::window::name::bellSettings)
    {
        addOptions(settingsOptionsList());
        buildInterface();
    }

    void BellSettingsWindow::buildInterface()
    {
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        optionsList->setPosition(bell_settings_style::settings_window::options_list_margin_x,
                                 bell_settings_style::settings_window::options_list_margin_y);
        optionsList->setMaximumWidth(bell_settings_style::settings_window::default_body_width);
        optionsList->setBoundaries(gui::Boundaries::Continuous);
    }

    std::list<Option> BellSettingsWindow::settingsOptionsList()
    {
        std::list<gui::Option> settingsOptionList;
        auto addWinSettings = [&](const UTF8 &name, const std::string &window) {
            settingsOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                name,
                [=](gui::Item &item) {
                    if (window.empty()) {
                        return false;
                    }
                    application->switchWindow(window, gui::ShowMode::GUI_SHOW_INIT);
                    return true;
                },
                [=](gui::Item &item) {
                    // put focus change callback here
                    return true;
                },
                this));
        };

        addWinSettings(utils::translate("app_bell_settings_home_view"), window::name::bellSettingsHomeView);
        addWinSettings(utils::translate("app_bell_settings_alarm_settings"), BellSettingsAlarmSettingsWindow::name);
        addWinSettings(utils::translate("app_bell_settings_bedtime_tone"), window::name::bellSettingsBedtimeTone);
        addWinSettings(utils::translate("app_bell_settings_advanced"), window::name::bellSettingsAdvanced);
        addWinSettings(utils::translate("app_bell_settings_turn_off"), window::name::bellSettingsTurnOff);

        return settingsOptionList;
    }

} // namespace gui
