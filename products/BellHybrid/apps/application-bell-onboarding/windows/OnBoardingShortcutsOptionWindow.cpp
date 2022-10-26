// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingShortcutsOptionWindow.hpp"
#include "BellOnBoardingNames.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <i18n/i18n.hpp>
#include <common/options/OptionBellMenu.hpp>

namespace gui
{
    OnBoardingShortcutsOptionWindow::OnBoardingShortcutsOptionWindow(app::ApplicationCommon *app,
                                                                     const std::string &name)
        : BellShortOptionWindow(app, name), startString{utils::translate("app_bell_onboarding_shortcuts_start")},
          skipString{utils::translate("app_bell_onboarding_shortcuts_skip")}
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bell_onboarding_shortcuts_question"));
    }

    std::list<Option> OnBoardingShortcutsOptionWindow::settingsOptionsList()
    {
        std::list<gui::Option> settingsOptionList;
        auto addWinSettings = [&](const UTF8 &name, std::function<bool(Item &)> activatedCallback) {
            settingsOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                name, activatedCallback, [=](gui::Item &) { return true; }, this));
        };

        addWinSettings(startString, [this](auto &) {
            getApplication()->switchWindow(gui::window::name::onBoardingShortcutsWindow);
            return true;
        });
        addWinSettings(skipString, [this](auto &) {
            getApplication()->switchWindow(gui::window::name::onBoardingSettingsWindow);
            return true;
        });

        return settingsOptionList;
    }
} /* namespace gui */
