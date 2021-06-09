﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/ApplicationSettings.hpp"
#include "DateAndTimeMainWindow.hpp"
#include "module-apps/application-desktop/windows/Names.hpp"
#include <time/DateAndTimeSettings.hpp>
#include "OptionSetting.hpp"
#include "service-appmgr/Controller.hpp"

namespace gui
{
    DateAndTimeMainWindow::DateAndTimeMainWindow(app::Application *app, std::string name)
        : BaseSettingsWindow(app, std::move(name))
    {
        setTitle(utils::translate("app_settings_date_and_time"));
        automaticDateAndTimeIsOn = utils::dateAndTimeSettings.isAutomaticDateAndTimeOn();
        automaticTimeZoneIsOn    = utils::dateAndTimeSettings.isAutomaticTimeZoneOn();
        timeFormat               = utils::dateAndTimeSettings.getTimeFormat();
        dateFormat               = utils::dateAndTimeSettings.getDateFormat();
        changeDateAndTimeWindow  = window::name::change_date_and_time;
    }

    auto DateAndTimeMainWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<Option> optionList;

        auto addOption = [&](const std::string &text, std::function<bool(Item &)> activatedCallback) {
            optionList.emplace_back(std::make_unique<option::OptionSettings>(
                text, activatedCallback, nullptr, nullptr, option::SettingRightItem::ArrowWhite));
        };
        auto addSwitchOption = [&](const std::string &text,
                                   std::function<bool(Item &)> activatedCallback,
                                   option::SettingRightItem rightItem,
                                   UTF8 textOnRight = UTF8()) {
            optionList.emplace_back(std::make_unique<option::OptionSettings>(
                text,
                activatedCallback,
                [=](Item &item) { return bottomBarCallback(item); },
                nullptr,
                rightItem,
                false,
                std::move(textOnRight)));
        };

        addSwitchOption(
            utils::translate("app_settings_date_and_time_automatic_date_and_time"),
            [=](Item &item) {
                automaticDateAndTimeIsOn = !automaticDateAndTimeIsOn;
                app::manager::Controller::changeAutomaticDateAndTimeIsOn(application, automaticDateAndTimeIsOn);
                refreshOptionsList();
                return true;
            },
            automaticDateAndTimeIsOn ? option::SettingRightItem::On : option::SettingRightItem::Off);

        if (!automaticDateAndTimeIsOn) {
            addOption(utils::translate("app_settings_date_and_time_change_date_and_time"), [=](Item &item) {
                LOG_INFO("switching to %s page", changeDateAndTimeWindow.c_str());
                application->switchWindow(changeDateAndTimeWindow, nullptr);
                return true;
            });
        }

        addSwitchOption(
            utils::translate("app_settings_date_and_time_automatic_time_zone"),
            [=](Item &item) {
                automaticTimeZoneIsOn = !automaticTimeZoneIsOn;
                app::manager::Controller::changeAutomaticTimeZoneIsOn(application, automaticTimeZoneIsOn);
                refreshOptionsList();
                return true;
            },
            automaticTimeZoneIsOn ? option::SettingRightItem::On : option::SettingRightItem::Off);

        if (!automaticTimeZoneIsOn) {
            addOption(utils::translate("app_settings_date_and_time_change_time_zone"), [=](Item &item) {
                LOG_INFO("switching to %s page", window::name::change_time_zone);
                application->switchWindow(window::name::change_time_zone, nullptr);
                return true;
            });
        }

        addSwitchOption(
            utils::translate("app_settings_date_and_time_time_format"),
            [=](Item &item) {
                timeFormat = (timeFormat == utils::time::Locale::TimeFormat::FormatTime12H)
                                 ? utils::time::Locale::TimeFormat::FormatTime24H
                                 : utils::time::Locale::TimeFormat::FormatTime12H;
                app::manager::Controller::changeTimeFormat(application, timeFormat);
                refreshOptionsList();
                return true;
            },
            option::SettingRightItem::Text,
            utils::time::Locale::get_time_format(timeFormat).data());

        addSwitchOption(
            utils::translate("app_settings_date_and_time_date_format"),
            [=](Item &item) {
                dateFormat = (dateFormat == utils::time::Locale::DateFormat::DD_MM_YYYY)
                                 ? utils::time::Locale::DateFormat::MM_DD_YYYY
                                 : utils::time::Locale::DateFormat::DD_MM_YYYY;
                app::manager::Controller::changeDateFormat(application, dateFormat);
                refreshOptionsList();
                return true;
            },
            option::SettingRightItem::Text,
            utils::time::Locale::get_date_format(dateFormat).data());

        return optionList;
    }

    bool DateAndTimeMainWindow::bottomBarCallback(Item &item)
    {
        if (item.focus) {
            setBottomBarText(utils::translate(style::strings::common::Switch), BottomBar::Side::CENTER);
        }
        else {
            setBottomBarText(utils::translate(style::strings::common::select), BottomBar::Side::CENTER);
        }
        return true;
    }
} // namespace gui
