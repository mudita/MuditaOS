// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusMainWindow.hpp"

#include "FocusSettingsWindow.hpp"
#include "FocusTimerWindow.hpp"

#include <common/options/OptionBellMenu.hpp>

namespace app::focus
{
    using namespace gui;
    FocusMainWindow::FocusMainWindow(app::ApplicationCommon *app, const std::string name)
        : BellOptionWindow(app, std::move(name))
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bellmain_focus_timer"));
    }

    std::list<Option> FocusMainWindow::settingsOptionsList()
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

        addWinSettings(utils::translate("app_bell_focus_start"), window::name::timer, defaultCallback);
        addWinSettings(utils::translate("app_bell_focus_settings"), window::name::settings, defaultCallback);

        return settingsOptionList;
    }
} // namespace app::focus
