// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateAndTimeMainWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>
#include <service-time/Constants.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/service-time/TimeMessage.hpp>

namespace gui
{
    DateAndTimeMainWindow::DateAndTimeMainWindow(app::ApplicationCommon *app, std::string name)
        : BaseSettingsWindow(app, std::move(name))
    {
        setTitle(utils::translate("app_settings_date_and_time"));

        automaticDateAndTimeIsOn = stm::api::isAutomaticDateAndTime();
        timeFormat               = stm::api::timeFormat();
        dateFormat               = stm::api::dateFormat();
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
                [=](Item &item) { return navBarCallback(item); },
                nullptr,
                rightItem,
                false,
                std::move(textOnRight)));
        };

        addSwitchOption(
            utils::translate("app_settings_date_and_time_automatic_date_and_time"),
            [=](Item &item) {
                automaticDateAndTimeIsOn = !automaticDateAndTimeIsOn;
                application->bus.sendUnicast(
                    std::make_shared<stm::message::SetAutomaticDateAndTimeRequest>(automaticDateAndTimeIsOn),
                    service::name::service_time);
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
                application->bus.sendUnicast(std::make_shared<stm::message::SetTimeFormatRequest>(timeFormat),
                                             service::name::service_time);
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
                application->bus.sendUnicast(std::make_shared<stm::message::SetDateFormatRequest>(dateFormat),
                                             service::name::service_time);
                refreshOptionsList();
                return true;
            },
            option::SettingRightItem::Text,
            utils::time::Locale::get_date_format(dateFormat).data());

        return optionList;
    }

    bool DateAndTimeMainWindow::navBarCallback(Item &item)
    {
        if (item.focus) {
            setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
        }
        else {
            setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
        }
        return true;
    }
} // namespace gui
