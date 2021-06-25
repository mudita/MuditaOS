// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsAdvancedWindow.hpp"
#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <application-bell-settings/data/BellSettingsStyle.hpp>
#include <apps-common/options/type/OptionBellMenu.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{

    BellSettingsAdvancedWindow::BellSettingsAdvancedWindow(app::Application *app)
        : OptionWindow(app, gui::window::name::bellSettingsAdvanced)
    {
        addOptions(settingsOptionsList());
        buildInterface();
    }

    void BellSettingsAdvancedWindow::buildInterface()
    {
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        optionsList->setPosition(bellSettingsStyle::settingsWindow::optionsListMarginX,
                                 bellSettingsStyle::settingsWindow::optionsListMarginY);
        optionsList->setMaximumWidth(bellSettingsStyle::settingsWindow::defaultBodyWidth);
        optionsList->setBoundaries(gui::Boundaries::Continuous);
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

        // for demo only - to be replaced by call o final window
        addWinSettings(utils::translate("app_bell_settings_advanced_time_units"),
                       gui::window::name::bellSettingsTimeUnits);
        addWinSettings(utils::translate("app_bell_settings_advanced_language"), gui::window::name::bellSettingsDialog);
        addWinSettings(utils::translate("app_bell_settings_advanced_about"), gui::window::name::bellSettingsDialog);
        addWinSettings(utils::translate("app_bell_settings_advanced_frontlight"),
                       gui::window::name::bellSettingsDialog);
        addWinSettings(utils::translate("app_bell_settings_advanced_factory_reset"),
                       gui::window::name::bellSettingsDialog);

        return settingsOptionList;
    }

} // namespace gui
