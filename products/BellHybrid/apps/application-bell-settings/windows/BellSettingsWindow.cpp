// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AboutYourBellWindow.hpp"
#include "BellSettingsFrontlightWindow.hpp"
#include "BellSettingsWindow.hpp"
#include "alarm_settings/BellSettingsAlarmSettingsMenuWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <data/BellSettingsStyle.hpp>

#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <common/popups/BellTurnOffOptionWindow.hpp>
#include <common/windows/BellFactoryReset.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <service-appmgr/Constants.hpp>
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

        auto factoryResetCallback = [this](const std::string &window) {
            auto actionCallback = [this]() {
                auto switchRequest = std::make_unique<app::manager::SwitchRequest>(
                    service::name::appmgr, app::applicationBellName, gui::BellFactoryReset::name, nullptr);
                application->bus.sendUnicast(std::move(switchRequest), service::name::appmgr);
                return true;
            };

            return [this, actionCallback, window](gui::Item &) {
                auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                    gui::DialogMetadata{utils::translate("app_bell_settings_factory_reset"),
                                        "",
                                        utils::translate("app_bell_settings_display_factory_reset_confirmation"),
                                        "",
                                        actionCallback});
                application->switchWindow(window, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
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

        addWinSettings(
            utils::translate("app_bell_settings_layout"), gui::window::name::bellSettingsLayout, defaultCallback);
        addWinSettings(utils::translate("app_bell_settings_alarm_settings"),
                       BellSettingsAlarmSettingsMenuWindow::name,
                       defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_bedtime_tone"), window::name::bellSettingsBedtimeTone, defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_time_units"), window::name::bellSettingsTimeUnits, defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_language"), gui::window::name::bellSettingsLanguage, defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_frontlight"), gui::BellSettingsFrontlightWindow::name, defaultCallback);
        addWinSettings(utils::translate("app_bell_settings_about"), gui::AboutYourBellWindow::name, defaultCallback);
        addWinSettings(
            utils::translate("app_bell_settings_turn_off"), BellTurnOffOptionWindow::defaultName, defaultCallback);
        addWinSettings(utils::translate("app_bell_settings_factory_reset"),
                       gui::window::name::bellSettingsFactoryReset,
                       factoryResetCallback);

        return settingsOptionList;
    }

} // namespace gui
