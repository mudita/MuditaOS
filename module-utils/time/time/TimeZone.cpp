// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeZone.hpp"
#include <utz/utz.h>
#include <utz/zones.h>
#include <log.hpp>
#include <ctime>

namespace utils::time
{
    constexpr uint16_t maxZoneToDisplayLength{27};
    constexpr uint16_t maxZoneRuleLength{50};
    constexpr uint8_t localTimeYearOffset{100};

    namespace
    {
        void copyTmToUdateTime(const tm *tm, udatetime_t *dt)
        {
            dt->date.year       = tm->tm_year - 100;
            dt->date.month      = tm->tm_mon;
            dt->date.dayofmonth = tm->tm_mday;
            dt->date.dayofweek  = tm->tm_wday;
            dt->time.hour       = tm->tm_hour;
            dt->time.minute     = tm->tm_min;
            dt->time.second     = tm->tm_sec;
        }

        void unpackDstRules(
            urule_packed_t *packedRules, char *dstRules, int8_t tzOffset, uint8_t currYear, bool isEndRule)
        {
            switch (packedRules->on_dayofmonth) {
            case 0: // the last day of week in month
                [[fallthrough]];
            case 1: // first day of week in month
                snprintf(dstRules,
                         maxZoneRuleLength,
                         ",M%d.%d.%d/%d",
                         packedRules->in_month,
                         packedRules->on_dayofmonth == 0 ? 5 : packedRules->on_dayofmonth,
                         packedRules->on_dayofweek % 7,
                         packedRules->at_hours + (isEndRule ? 1 : 0) + (packedRules->at_is_local_time ? 0 : tzOffset));
                break;
            default: // The Julian day n (1 <= n <= 365)
                snprintf(dstRules,
                         maxZoneRuleLength,
                         ",J%d/%d",
                         get_day_in_year(packedRules, currYear),
                         packedRules->at_hours + (isEndRule ? 1 : 0) + (packedRules->at_is_local_time ? 0 : tzOffset));
            }
        }
    } // namespace

    [[nodiscard]] auto getAvailableTimeZonesWithOffset() -> std::vector<std::string>
    {
        std::vector<std::string> timeZonesNames;
        char zoneToDisplay[maxZoneToDisplayLength];
        auto zonePointer = reinterpret_cast<const char *>(zone_names);
        uzone_t zoneOut;

        for (uint16_t zone = 0; zone < NUM_ZONE_NAMES; zone++) {
            auto pointerToZoneName = zonePointer;
            unpack_zone(&zone_defns[get_next(&zonePointer)], pointerToZoneName, &zoneOut);

            snprintf(zoneToDisplay,
                     maxZoneToDisplayLength,
                     "UTC%s%02d:%02d %s",
                     zoneOut.offset.hours > 0 ? "+" : "-",
                     std::abs(zoneOut.offset.hours),
                     zoneOut.offset.minutes,
                     zoneOut.name);
            timeZonesNames.push_back(zoneToDisplay);
            zonePointer++;
        }

        return timeZonesNames;
    }

    [[nodiscard]] auto getTimeZoneOffset(const std::string &zoneName, const time_t &time) -> std::chrono::seconds
    {
        uzone_t zoneOut;

        if (zoneName.empty() || get_zone_by_name(const_cast<char *>(zoneName.c_str()), &zoneOut)) {
            LOG_ERROR("Zone %s not found", zoneName.c_str());
            return std::chrono::seconds(0);
        }

        auto gmTime = gmtime(&time);
        udatetime_t udateTime;
        copyTmToUdateTime(gmTime, &udateTime);

        uoffset_t offset;
        get_current_offset(&zoneOut, &udateTime, &offset);

        return std::chrono::seconds(std::chrono::hours(offset.hours) + std::chrono::minutes(offset.minutes));
    }

    [[nodiscard]] auto getTimeZoneRules(const std::string &zoneName) -> std::string
    {
        std::string zoneRules;
        char offsetRules[maxZoneRuleLength];
        urule_packed_t startRules, endRules;
        uzone_t zoneOut;
        time_t tm           = std::time(nullptr);
        uint8_t currentYear = localtime(&tm)->tm_year - localTimeYearOffset;

        if (get_zone_by_name(const_cast<char *>(zoneName.c_str()), &zoneOut)) {
            LOG_ERROR("Zone %s not found", zoneName.c_str());
            return "";
        }

        snprintf(offsetRules, maxZoneRuleLength, "UTC%d:%02d", -zoneOut.offset.hours, zoneOut.offset.minutes);

        zoneRules = offsetRules;

        if (!get_zone_rules(zoneOut.src, currentYear, &startRules, &endRules)) {
            char dstRules[maxZoneRuleLength];

            zoneRules.append("UTC");
            unpackDstRules(&startRules, dstRules, zoneOut.offset.hours, currentYear, false);
            zoneRules.append(dstRules);

            unpackDstRules(&endRules, dstRules, zoneOut.offset.hours, currentYear, true);
            zoneRules.append(dstRules);
        }

        return zoneRules;
    }

} // namespace utils::time
