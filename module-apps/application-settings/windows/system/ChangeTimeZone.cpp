// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChangeTimeZone.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>
#include <service-time/Constants.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <time/TimeZone.hpp>

#include <algorithm>

namespace gui
{
    namespace
    {
        constexpr auto NegativeOffsetPrefix = "UTC-";
    } // namespace

    ChangeTimeZone::ChangeTimeZone(app::ApplicationCommon *app)
        : BaseSettingsWindow(app, window::name::change_date_and_time), timeZonesList(sortTimeZones(getTimeZones()))
    {
        setTitle(utils::translate("app_settings_date_and_time_time_zone"));
    }

    auto ChangeTimeZone::getTimeZones() -> std::vector<std::string>
    {
        return utils::time::getAvailableTimeZonesWithOffset();
    }

    auto ChangeTimeZone::sortTimeZones(std::vector<std::string> &&timeZones) -> std::vector<std::string>
    {
        std::vector<std::string> zones = std::move(timeZones);
        auto pivot                     = std::partition(zones.begin(), zones.end(), [](const auto &tz) {
            return tz.rfind(NegativeOffsetPrefix, 0) == 0; // starts with NegativeOffsetPrefix
        });
        std::sort(zones.begin(), pivot, std::greater<std::string>{});
        std::sort(pivot, zones.end());
        return zones;
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
