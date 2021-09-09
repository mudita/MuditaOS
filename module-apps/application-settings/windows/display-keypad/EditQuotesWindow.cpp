// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

        auto addCheckOption = [&](UTF8 text, bool &switcher) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                text,
                [&](gui::Item &item) mutable {
                    switchHandler(switcher);
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
                switcher ? gui::option::SettingRightItem::Checked : gui::option::SettingRightItem::Disabled));
        };

        addCheckOption(utils::translate("app_settings_display_wallpaper_quotes_our_favourites"),
                       isOurFavouritesSwitchOn);

        if (isOurFavouritesSwitchOn) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translate("app_settings_display_wallpaper_quotes_categories"),
                [=](gui::Item &item) {
                    application->switchWindow(gui::window::name::quote_categories, nullptr);
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

        addCheckOption(utils::translate("app_settings_display_wallpaper_quotes_custom"), isCustomSwitchOn);

        if (isCustomSwitchOn) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translate("app_settings_display_wallpaper_edit_quotes"),
                [=](gui::Item &item) {
                    application->switchWindow(gui::window::name::quotes, nullptr);
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

    void EditQuotesWindow::switchHandler(bool &optionSwitch)
    {
        isOurFavouritesSwitchOn = false;
        isCustomSwitchOn        = false;

        optionSwitch = !optionSwitch;

        refreshOptionsList();
    }

} // namespace gui
