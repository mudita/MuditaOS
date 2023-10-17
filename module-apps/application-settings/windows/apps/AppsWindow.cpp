// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AppsWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

namespace gui
{
    AppsWindow::AppsWindow(app::ApplicationCommon *app) : OptionWindow(app, gui::window::name::apps)
    {
        addOptions(appsAndToolsOptionsList());
        setTitle(utils::translate("app_settings_apps"));
    }

    std::list<Option> AppsWindow::appsAndToolsOptionsList()
    {
        std::list<gui::Option> optionList;

        auto i18     = [](std::string text) { return utils::translate(text); };
        auto addMenu = [&](UTF8 name, std::string window) {
            optionList.emplace_back(gui::Option{name,
                                                [=](gui::Item &item) {
                                                    if (window.empty()) {
                                                        return false;
                                                    }
                                                    LOG_INFO("switching to %s page", window.c_str());
                                                    application->switchWindow(window, nullptr);
                                                    return true;
                                                },
                                                gui::option::Arrow::Enabled});
        };

        addMenu(i18("app_settings_apps_phone"), gui::window::name::phone);
        addMenu(i18("app_settings_apps_messages"), gui::window::name::messages);
        addMenu(i18("app_settings_apps_alarm_clock"), gui::window::name::alarm_clock);

        return optionList;
    }
} // namespace gui
