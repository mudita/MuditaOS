// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LockedScreenWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"

#include <i18n/i18n.hpp>

namespace gui
{

    LockedScreenWindow::LockedScreenWindow(app::Application *app) : BaseSettingsWindow(app, window::name::locked_screen)
    {
        setTitle(utils::localize.get("app_settings_display_locked_screen"));
    }

    auto LockedScreenWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<gui::Option> optionList;

        auto addMenu = [&](UTF8 name, std::string window = "") {
            optionList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                name,
                [=](gui::Item &item) {
                    if (window.empty()) {
                        return false;
                    }
                    LOG_INFO("switching to %s page", window.c_str());
                    application->switchWindow(window, nullptr);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                gui::option::SettingRightItem::ArrowWhite));
        };

        addMenu(utils::translateI18("app_settings_display_locked_screen_autolock"), gui::window::name::autolock);
        addMenu(utils::translateI18("app_settings_display_locked_screen_wallpaper"), gui::window::name::wallpaper);

        return optionList;
    }

} // namespace gui
