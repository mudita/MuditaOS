// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "BellSettingsAdvancedWindow.hpp"
#include "BellSettingsStyle.hpp"

#include <common/options/OptionBellMenu.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/windows/Dialog.hpp>

namespace gui
{

    BellSettingsAdvancedWindow::BellSettingsAdvancedWindow(app::ApplicationCommon *app)
        : BellOptionWindow(app, gui::window::name::bellSettingsAdvanced)
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bell_settings_advanced"));
    }

    std::list<Option> BellSettingsAdvancedWindow::settingsOptionsList()
    {
        std::list<gui::Option> settingsOptionList;
        auto addWinSettings = [&](const UTF8 &name, const std::string &window) {
            settingsOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
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

        addWinSettings(utils::translate("app_bell_settings_advanced_time_units"),
                       gui::window::name::bellSettingsTimeUnits);
        addWinSettings(utils::translate("app_bell_settings_advanced_frontlight"),
                       gui::window::name::bellSettingsFrontlight);

        return settingsOptionList;
    }

} // namespace gui
