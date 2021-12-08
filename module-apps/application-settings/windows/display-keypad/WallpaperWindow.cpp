// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{

    namespace
    {
        const std::vector<std::pair<std::string, WallpaperOption>> wallpaperOptions = {
            {"app_settings_display_wallpaper_logo", WallpaperOption::Logo},
            {"app_settings_display_wallpaper_clock", WallpaperOption::Clock},
            {"app_settings_display_wallpaper_quotes", WallpaperOption::Quote}};
    } // namespace

    WallpaperWindow::WallpaperWindow(app::ApplicationCommon *app, app::settingsInterface::WallpaperSettings *settings)
        : BaseSettingsWindow(app, window::name::wallpaper), optionModel{
                                                                std::make_unique<WallpaperOptionModel>(settings)}
    {
        setTitle(utils::translate("app_settings_display_wallpaper"));
    }

    auto WallpaperWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        for (const auto &[text, option] : wallpaperOptions) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translate(text),
                [&](gui::Item &item) mutable {
                    optionModel->saveValue(option);
                    refreshOptionsList();
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
                    }
                    return true;
                },
                this,
                optionModel->isCurrentOption(option) ? gui::option::SettingRightItem::Checked
                                                     : gui::option::SettingRightItem::Disabled));
        }

        if (optionModel->isQuoteOptionSelected()) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translate("app_settings_display_wallpaper_edit_quotes"),
                [=](gui::Item &item) {
                    application->switchWindow(gui::window::name::edit_quotes, nullptr);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
                    }
                    return true;
                },
                this,
                gui::option::SettingRightItem::ArrowWhite));
        }

        return optionsList;
    }

    void WallpaperWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        optionModel->update();
        refreshOptionsList();
    }
} // namespace gui
