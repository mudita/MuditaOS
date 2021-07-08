// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsWindow.hpp"

#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <data/BellSettingsStyle.hpp>

#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/options/type/OptionBellMenu.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <service-appmgr/Controller.hpp>

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

        optionsList->setPosition(bellSettingsStyle::settingsWindow::optionsListMarginX,
                                 bellSettingsStyle::settingsWindow::optionsListMarginY);
        optionsList->setMaximumWidth(bellSettingsStyle::settingsWindow::defaultBodyWidth);
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

        // for demo only - to be replaced by call o final window
        addWinSettings(utils::translate("app_bell_settings_home_view"), gui::window::name::bellSettingsDialog);
        addWinSettings(utils::translate("app_bell_settings_alarm_setup"), gui::window::name::bellSettingsDialog);
        addWinSettings(utils::translate("app_bell_settings_evening_rtone"), gui::window::name::bellSettingsDialog);
        addWinSettings(utils::translate("app_bell_settings_turn_off"), gui::window::name::bellSettingsDialog);
        addWinSettings(utils::translate("app_bell_settings_advanced"), gui::window::name::bellSettingsAdvanced);

        return settingsOptionList;
    }

} // namespace gui
