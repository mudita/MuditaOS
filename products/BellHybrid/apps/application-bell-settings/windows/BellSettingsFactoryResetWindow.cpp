// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "BellSettingsFactoryResetWindow.hpp"

#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/data/StyleCommon.hpp>
#include <common/windows/BellFactoryReset.hpp>
#include <i18n/i18n.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>
#include <service-appmgr/messages/SwitchRequest.hpp>

namespace gui
{
    BellSettingsFactoryResetWindow::BellSettingsFactoryResetWindow(app::ApplicationCommon *app, const std::string &name)
        : BellOptionWithDescriptionWindow(app, name)
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bell_settings_factory_reset"));
        setListDescription(utils::translate("app_bell_settings_display_factory_reset_confirmation"));
    }

    std::list<Option> BellSettingsFactoryResetWindow::settingsOptionsList()
    {
        std::list<gui::Option> settingsOptionList;

        auto addWinSettings = [&](const UTF8 &name, std::function<bool(Item &)> activatedCallback) {
            settingsOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                name, activatedCallback, []([[maybe_unused]] gui::Item &item) { return true; }, this));
        };

        addWinSettings(utils::translate("app_bell_settings_factory_reset_option_back"),
                       [this]([[maybe_unused]] Item &item) {
                           application->returnToPreviousWindow();
                           return true;
                       });
        addWinSettings(utils::translate("app_bell_settings_factory_reset_option_reset"),
                       [this]([[maybe_unused]] Item &item) {
                           auto switchRequest = std::make_unique<app::manager::SwitchRequest>(
                               service::name::appmgr, app::applicationBellName, gui::BellFactoryReset::name, nullptr);
                           application->bus.sendUnicast(std::move(switchRequest), service::name::appmgr);
                           return true;
                       });

        return settingsOptionList;
    }

    void BellSettingsFactoryResetWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}
} /* namespace gui */
