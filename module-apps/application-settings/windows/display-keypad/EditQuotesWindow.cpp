// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EditQuotesWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{
    EditQuotesWindow::EditQuotesWindow(app::ApplicationCommon *app) : BaseSettingsWindow(app, window::name::edit_quotes)
    {
        setTitle(utils::translate("app_settings_display_wallpaper_edit_quotes"));
    }

    auto EditQuotesWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_display_wallpaper_quotes_categories"),
            [=](gui::Item &item) {
                application->switchWindow(gui::window::name::quote_categories, nullptr);
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

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_display_wallpaper_edit_custom_quotes"),
            [=](gui::Item &item) {
                application->switchWindow(gui::window::name::quotes, nullptr);
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

        return optionsList;
    }

} // namespace gui
