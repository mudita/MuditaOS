// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    WallpaperWindow::WallpaperWindow(app::Application *app) : BaseSettingsWindow(app, window::name::wallpaper)
    {
        setTitle(utils::localize.get("app_settings_display_locked_screen_wallpaper"));
    }

    auto WallpaperWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        auto addCheckOption = [&](UTF8 text, bool &Switch) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                text,
                [&](gui::Item &item) mutable {
                    switchHandler(Switch);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                Switch ? gui::option::SettingRightItem::Checked : gui::option::SettingRightItem::Disabled));
        };

        addCheckOption(utils::translateI18("app_settings_display_wallpaper_logo"), isWallpaperLogoSwitchOn);
        addCheckOption(utils::translateI18("app_settings_display_wallpaper_clock"), isWallpaperClockSwitchOn);
        addCheckOption(utils::translateI18("app_settings_display_wallpaper_quotes"), isWallpaperQuotesSwitchOn);

        if (isWallpaperQuotesSwitchOn) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translateI18("app_settings_display_wallpaper_select_quotes"),
                [=](gui::Item &item) {
                    application->switchWindow(gui::window::name::quotes, nullptr);
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
        }

        return optionsList;
    }

    void WallpaperWindow::switchHandler(bool &optionSwitch)
    {
        isWallpaperQuotesSwitchOn = false;
        isWallpaperClockSwitchOn  = false;
        isWallpaperLogoSwitchOn   = false;

        optionSwitch = !optionSwitch;
        rebuildOptionList();
    }

} // namespace gui
