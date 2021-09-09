// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{
    WallpaperWindow::WallpaperWindow(app::ApplicationCommon *app) : BaseSettingsWindow(app, window::name::wallpaper)
    {
        setTitle(utils::translate("app_settings_display_wallpaper"));
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
                        this->setBottomBarText(utils::translate(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                Switch ? gui::option::SettingRightItem::Checked : gui::option::SettingRightItem::Disabled));
        };

        addCheckOption(utils::translate("app_settings_display_wallpaper_logo"), isWallpaperLogoSwitchOn);
        addCheckOption(utils::translate("app_settings_display_wallpaper_clock"), isWallpaperClockSwitchOn);
        addCheckOption(utils::translate("app_settings_display_wallpaper_quotes"), isWallpaperQuotesSwitchOn);

        if (isWallpaperQuotesSwitchOn) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translate("app_settings_display_wallpaper_edit_quotes"),
                [=](gui::Item &item) {
                    application->switchWindow(gui::window::name::edit_quotes, nullptr);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translate(style::strings::common::select),
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
        refreshOptionsList();
    }

} // namespace gui
