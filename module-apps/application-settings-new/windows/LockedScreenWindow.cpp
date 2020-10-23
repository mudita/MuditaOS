// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LockedScreenWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "i18/i18.hpp"

namespace gui
{

    LockedScreenWindow::LockedScreenWindow(app::Application *app) : BaseSettingsWindow(app, window::name::locked_screen)
    {
        buildInterface();
    }

    void LockedScreenWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        addOptions(lockedScreenOptions());
        setTitle(utils::localize.get("app_settings_display_locked_screen"));
    }

    std::list<Option> LockedScreenWindow::lockedScreenOptions()
    {
        std::list<gui::Option> optionList;

        auto i18     = [](std::string text) { return utils::localize.get(text); };
        auto addMenu = [&](UTF8 name, std::string window = "") {
            optionList.emplace_back(gui::Option{name,
                                                [=](gui::Item &item) {
                                                    if (window.empty()) {
                                                        return false;
                                                    }
                                                    LOG_INFO("switching to %s page", window.c_str());
                                                    application->switchWindow(window, nullptr);
                                                    return true;
                                                },
                                                gui::Arrow::Enabled});
        };

        addMenu(i18("app_settings_display_locked_screen_autolock"), gui::window::name::autolock);
        addMenu(i18("app_settings_display_locked_screen_wallpaper"), gui::window::name::wallpaper);

        return optionList;
    }

} // namespace gui
