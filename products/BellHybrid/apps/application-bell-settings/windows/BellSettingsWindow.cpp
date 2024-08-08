// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AboutYourBellWindow.hpp"
#include "BellSettingsFrontlightWindow.hpp"
#include "BellSettingsBedsideWindow.hpp"
#include "BellSettingsFactoryResetWindow.hpp"
#include "BellSettingsWindow.hpp"
#include "alarm_settings/BellSettingsAlarmSettingsMenuWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>

#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <common/popups/BellTurnOffOptionWindow.hpp>
#include <common/windows/BellFactoryReset.hpp>
#include <service-appmgr/messages/SwitchRequest.hpp>

namespace gui
{
    BellSettingsWindow::BellSettingsWindow(app::ApplicationCommon *app)
        : BellOptionWindow(app, gui::window::name::bellSettings)
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bellmain_settings"));
    }

    std::list<Option> BellSettingsWindow::settingsOptionsList()
    {
        using ActivatedCallback = std::function<bool(gui::Item &)>;
        using Callback          = std::function<ActivatedCallback(const std::string &window)>;

        auto defaultCallback = [this](const std::string &window) {
            return [window, this](gui::Item &) {
                if (window.empty()) {
                    return false;
                }
                application->switchWindow(window);
                return true;
            };
        };

        std::list<gui::Option> settingsOptionList;
        auto addWinSettings = [&](const UTF8 &name, const std::string &window, Callback &&callback) {
            settingsOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                name,
                callback(window),
                [=](gui::Item &item) {
                    // put focus change callback here
                    return true;
                },
                this));
        };
        const std::string bellSettingsTimeUnitsTitle{utils::translate("app_calllog_date") + " & " +
                                                     utils::translate("app_bell_settings_time_units")};

        addWinSettings(
            utils::translate("app_bell_settings_layout"), gui::window::name::bellSettingsLayout, defaultCallback);
        addWinSettings(utils::translate("app_bell_settings_alarm_settings"),
                       BellSettingsAlarmSettingsMenuWindow::name,
                       defaultCallback);
        addWinSettings(utils::translate("app_bell_settings_relaxation_fade"),
                       window::name::bellSettingsRelaxation,
                       defaultCallback);
        addWinSettings(utils::translate("app_bell_settings_bedtime_chime"),
                       window::name::bellSettingsBedtimeTone,
                       defaultCallback);
        addWinSettings(bellSettingsTimeUnitsTitle, window::name::bellSettingsTimeUnits, defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_language"), gui::window::name::bellSettingsLanguage, defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_frontlight"), gui::BellSettingsFrontlightWindow::name, defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_bedside_lamp"), gui::BellSettingsBedsideWindow::name, defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_shortcuts"), window::name::bellSettingsShortcuts, defaultCallback);
        addWinSettings(utils::translate("app_bell_update_instruction_title"),
                       window::name::bellSettingsUpdateInstruction,
                       defaultCallback);
        addWinSettings(utils::translate("app_bell_settings_about"), gui::AboutYourBellWindow::name, defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_turn_off"), BellTurnOffOptionWindow::defaultName, defaultCallback);
        addWinSettings(utils::translate("app_bell_settings_factory_reset"),
                       gui::BellSettingsFactoryResetWindow::defaultName,
                       defaultCallback);

        return settingsOptionList;
    }
} // namespace gui
