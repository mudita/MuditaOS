// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChangeTimeZone.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>
#include <service-time/Constants.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <time/TimeZone.hpp>

namespace gui
{
    ChangeTimeZone::ChangeTimeZone(app::Application *app)
        : BaseSettingsWindow(app, window::name::change_date_and_time),
          timeZonesList(utils::time::getAvailableTimeZonesWithOffset())
    {
        setTitle(utils::translate("app_settings_date_and_time_time_zone"));
    }

    void ChangeTimeZone::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        selectedTimeZone = stm::api::getCurrentTimezoneName();
        refreshOptionsList(setTimeZoneIndex());
    }

    [[nodiscard]] auto ChangeTimeZone::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> options;

        for (const auto &zone : timeZonesList) {
            options.emplace_back(std::make_unique<gui::option::OptionSettings>(
                zone,
                [=](const gui::Item &item) {
                    selectedTimeZone = extractTimeZoneName(zone);
                    application->bus.sendUnicast(std::make_shared<stm::message::SetTimezoneRequest>(selectedTimeZone),
                                                 service::name::service_time);
                    refreshOptionsList(setTimeZoneIndex());
                    return true;
                },
                nullptr,
                this,
                selectedTimeZone == extractTimeZoneName(zone) ? gui::option::SettingRightItem::Checked
                                                              : gui::option::SettingRightItem::Disabled));
        }

        return options;
    }

    [[nodiscard]] auto ChangeTimeZone::setTimeZoneIndex() -> unsigned int
    {
        unsigned int zoneIndex = 0;
        if (selectedTimeZone.empty()) {
            selectedTimeZone.assign(utils::time::defaultTimeZoneName);
        }
        for (zoneIndex = 0; zoneIndex < timeZonesList.size(); ++zoneIndex) {
            if (selectedTimeZone == extractTimeZoneName(timeZonesList[zoneIndex])) {
                break;
            }
        }
        return zoneIndex;
    }

    [[nodiscard]] auto ChangeTimeZone::extractTimeZoneName(const std::string &name) const noexcept -> std::string
    {
        return name.substr(utils::time::timeZoneNameOffset);
    }

} // namespace gui
