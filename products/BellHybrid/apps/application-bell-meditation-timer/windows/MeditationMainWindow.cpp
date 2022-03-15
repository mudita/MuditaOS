// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationMainWindow.hpp"

#include "SettingsWindow.hpp"
#include "StatisticsWindow.hpp"
#include "MeditationTimerWindow.hpp"

#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <service-appmgr/messages/SwitchRequest.hpp>

namespace app::meditation
{
    using namespace gui;
    MeditationMainWindow::MeditationMainWindow(app::ApplicationCommon *app)
        : BellOptionWindow(app, gui::name::window::main_window)
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bell_meditation_timer"));
    }

    std::list<Option> MeditationMainWindow::settingsOptionsList()
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

        addWinSettings(utils::translate("app_bell_meditation_start"), MeditationTimerWindow::name, defaultCallback);
        addWinSettings(utils::translate("app_bell_meditation_settings"), SettingsWindow::name, defaultCallback);
        addWinSettings(utils::translate("app_bell_meditation_statistics"), StatisticsWindow::name, defaultCallback);

        return settingsOptionList;
    }

} // namespace app::meditation
