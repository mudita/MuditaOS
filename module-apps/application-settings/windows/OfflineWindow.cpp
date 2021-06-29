// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OfflineWindow.hpp"

#include "ApplicationSettings.hpp"
#include "SettingsStyle.hpp"

#include <i18n/i18n.hpp>
#include <OptionSetting.hpp>

namespace gui
{

    OfflineWindow::OfflineWindow(app::Application *app, app::settingsInterface::OfflineSettings *offlineSettings)
        : BaseSettingsWindow(app, gui::window::name::offline), offlineSettings(offlineSettings)
    {
        isFlightMode = offlineSettings->isFlightMode();
        buildInterface();
    }

    void OfflineWindow::buildInterface()
    {
        setTitle(utils::translate("app_settings_title_offline"));
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
        descriptionText->setText(utils::translate(isFlightMode ? "app_settings_info_offline_flight_mode"
                                                               : "app_settings_info_offline_messages_only"));
        descriptionText->setVisible(true);
    }

    auto OfflineWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;
        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_allow"),
            [=](gui::Item &item) { return changeFlightMode(!isFlightMode); },
            nullptr,
            nullptr,
            gui::option::SettingRightItem::Text,
            false,
            utils::translate(isFlightMode ? "app_settings_no_network_connection_flight_mode"
                                          : "app_settings_messages_only"),
            true));

        if (!isFlightMode) {
            optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translate("app_settings_title_connection_frequency"),
                [=](gui::Item &item) {
                    this->application->switchWindow(gui::window::name::connection_frequency, nullptr);
                    return true;
                },
                nullptr,
                nullptr,
                gui::option::SettingRightItem::ArrowWhite));
        }

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::Switch));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        return optList;
    }

    bool OfflineWindow::changeFlightMode(bool isFlightMode)
    {
        this->isFlightMode = isFlightMode;
        offlineSettings->setFlightMode(isFlightMode);
        descriptionText->setText(utils::translate(isFlightMode ? "app_settings_info_offline_flight_mode"
                                                               : "app_settings_info_offline_messages_only"));
        rebuildOptionList();
        return true;
    }

} // namespace gui
