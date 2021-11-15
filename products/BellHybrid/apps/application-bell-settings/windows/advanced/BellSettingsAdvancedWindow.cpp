// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "BellSettingsAdvancedWindow.hpp"
#include "BellSettingsStyle.hpp"
#include "BellSettingsFrontlightWindow.hpp"

#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/messages/UserPowerDownRequest.hpp>
#include <service-appmgr/messages/SwitchRequest.hpp>
#include <windows/advanced/AboutYourBellWindow.hpp>
#include <common/windows/BellFactoryReset.hpp>
#include <common/windows/BellTurnOffWindow.hpp>

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
        using ActivatedCallback = std::function<bool(gui::Item &)>;
        using Callback          = std::function<ActivatedCallback(const std::string &window)>;

        std::list<gui::Option> settingsOptionList;

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
                    gui::DialogMetadata{utils::translate("app_bell_settings_advanced_factory_reset"),
                                        "",
                                        utils::translate("app_bell_settings_display_factory_reset_confirmation"),
                                        "",
                                        actionCallback});
                application->switchWindow(window, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
                return true;
            };
        };

        auto addOption = [&settingsOptionList, this](const UTF8 &name, const std::string &window, Callback &&callback) {
            settingsOptionList.emplace_back(
                std::make_unique<gui::option::OptionBellMenu>(name, callback(window), nullptr, this));
        };

        addOption(utils::translate("app_bell_settings_advanced_language"),
                  gui::window::name::bellSettingsLanguage,
                  defaultCallback);
        addOption(utils::translate("app_bell_settings_advanced_frontlight"),
                  gui::BellSettingsFrontlightWindow::name,
                  defaultCallback);
        addOption(
            utils::translate("app_bell_settings_advanced_about"), gui::AboutYourBellWindow::name, defaultCallback);
        addOption(utils::translate("app_bell_settings_advanced_factory_reset"),
                  gui::window::name::bellSettingsFactoryReset,
                  factoryResetCallback);

        return settingsOptionList;
    }

} // namespace gui
