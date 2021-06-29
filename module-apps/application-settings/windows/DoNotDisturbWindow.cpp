// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationSettings.hpp"
#include "DoNotDisturbWindow.hpp"
#include "SettingsStyle.hpp"

#include <i18n/i18n.hpp>
#include <OptionSetting.hpp>

namespace gui
{
    DoNotDisturbWindow::DoNotDisturbWindow(app::Application *app, app::settingsInterface::DndSettings *dndSettings)
        : BaseSettingsWindow(app, gui::window::name::do_not_disturb), dndSettings(dndSettings)
    {
        buildInterface();
    }

    void DoNotDisturbWindow::buildInterface()
    {
        setTitle(utils::translate("app_settings_title_do_not_disturb"));
        optionsList->setSize(optionsList->getWidth(),
                             optionsList->getHeight() - style::settings::window::offline::body_offset);
        bar = new Rect(this,
                       style::window::default_left_margin,
                       style::settings::window::offline::bar_y,
                       style::window::default_body_width,
                       style::settings::window::offline::bar_h);
        bar->setVisible(true);
        descriptionText = new Text(this,
                                   style::window::default_left_margin,
                                   style::settings::window::offline::description_y,
                                   style::window::default_body_width,
                                   style::settings::window::offline::description_h);
        descriptionText->setFont(style::window::font::medium);
        descriptionText->setText(utils::translate("app_settings_info_dnd"));
        descriptionText->setVisible(true);
    }

    auto DoNotDisturbWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;
        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_notifications_when_locked"),
            [=](gui::Item &item) {
                dndSettings->setNotificationsWhenLocked(!dndSettings->getNotificationsWhenLocked());
                refreshOptionsList();
                return true;
            },
            nullptr,
            this,
            (dndSettings->getNotificationsWhenLocked()) ? gui::option::SettingRightItem::On
                                                        : gui::option::SettingRightItem::Off));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_calls_from_favorites"),
            [=](gui::Item &item) {
                dndSettings->setCallsFromFavourite(!dndSettings->getCallsFromFavourite());
                refreshOptionsList();
                return true;
            },
            nullptr,
            this,
            (dndSettings->getCallsFromFavourite()) ? gui::option::SettingRightItem::On
                                                   : gui::option::SettingRightItem::Off));

        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::Switch));
        return optList;
    }
} // namespace gui
