// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DoNotDisturbWindow.hpp"

#include <application-settings/ApplicationSettings.hpp>
#include <application-settings/widgets/SettingsStyle.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{
    DoNotDisturbWindow::DoNotDisturbWindow(app::ApplicationCommon *app,
                                           app::settingsInterface::DndSettings *dndSettings)
        : BaseSettingsWindow(app, gui::window::name::do_not_disturb), dndSettings(dndSettings)
    {
        buildInterface();
    }

    void DoNotDisturbWindow::buildInterface()
    {
        setTitle(utils::translate("app_settings_title_do_not_disturb"));
        optionsList->setSize(optionsList->getWidth(),
                             optionsList->getHeight() - style::settings::window::phone_modes::body_offset);

        descriptionText = new gui::Text(this,
                                        style::window::default_left_margin,
                                        style::settings::window::phone_modes::description_y,
                                        style::window::default_body_width,
                                        style::settings::window::phone_modes::description_h);
        descriptionText->setFont(style::window::font::medium);
        descriptionText->setAlignment(gui::Alignment{gui::Alignment::Vertical::Top});
        descriptionText->setEdges(gui::RectangleEdge::Top);
        descriptionText->setPenWidth(style::window::default_border_rect_no_focus);
        descriptionText->setPadding(gui::Padding(style::settings::window::phone_modes::description_vertical_padding,
                                                 style::settings::window::phone_modes::description_top_padding,
                                                 style::settings::window::phone_modes::description_vertical_padding,
                                                 0));
        descriptionText->setText(utils::translate("app_settings_info_dnd"));
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

        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::Switch));
        return optList;
    }
} // namespace gui
