// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "time/TimeRangeParser.hpp"
#include <module-apps/application-calendar/data/dateCommon.hpp>
#include <module-db/Interface/EventsRecord.hpp>

#include <cstdint>
#include <cstring>
#include <iostream>

TEST_CASE("Time display parser")
{
    using utils::time::Version;
    SECTION("After noon time input (mode12h)")
    {
        auto event      = EventsRecord();
        event.date_from = TimePointFromString("2020-10-10 14:24:00");
        event.date_till = TimePointFromString("2020-10-10 15:36:00");

        auto returnString = utils::time::TimeRangeParser().getCalendarTimeString(
            event.date_from, event.date_till, Version::DayEvent, false);
        REQUIRE("2020-10-10 14:24:00" == TimePointToString(event.date_from));
        REQUIRE("2020-10-10 15:36:00" == TimePointToString(event.date_till));
        REQUIRE("2" == TimePointToHourString12H(event.date_from));
        REQUIRE("24" == TimePointToMinutesString(event.date_from));
        REQUIRE("3" == TimePointToHourString12H(event.date_till));
        REQUIRE("36" == TimePointToMinutesString(event.date_till));
        REQUIRE(returnString == "2:24 - 3:36 " + utils::localize.get(utils::time::Locale::getPM()));
    }

    SECTION("Before noon time input (mode12h)")
    {
        auto event      = EventsRecord();
        event.date_from = TimePointFromString("2020-12-31 05:59:00");
        event.date_till = TimePointFromString("2020-12-31 07:45:00");

        auto returnString = utils::time::TimeRangeParser().getCalendarTimeString(
            event.date_from, event.date_till, Version::DayEvent, false);
        REQUIRE("2020-12-31 05:59:00" == TimePointToString(event.date_from));
        REQUIRE("2020-12-31 07:45:00" == TimePointToString(event.date_till));
        REQUIRE("5" == TimePointToHourString12H(event.date_from));
        REQUIRE("59" == TimePointToMinutesString(event.date_from));
        REQUIRE("7" == TimePointToHourString12H(event.date_till));
        REQUIRE("45" == TimePointToMinutesString(event.date_till));
        REQUIRE(returnString == "5:59 - 7:45 " + utils::localize.get(utils::time::Locale::getAM()));
    }

    SECTION("Mixed time input (mode12h)")
    {
        auto event      = EventsRecord();
        event.date_from = TimePointFromString("2021-01-01 01:05:00");
        event.date_till = TimePointFromString("2021-01-01 19:55:00");

        auto returnString = utils::time::TimeRangeParser().getCalendarTimeString(
            event.date_from, event.date_till, Version::DayEvent, false);
        REQUIRE("2021-01-01 01:05:00" == TimePointToString(event.date_from));
        REQUIRE("2021-01-01 19:55:00" == TimePointToString(event.date_till));
        REQUIRE("1" == TimePointToHourString12H(event.date_from));
        REQUIRE("05" == TimePointToMinutesString(event.date_from));
        REQUIRE("7" == TimePointToHourString12H(event.date_till));
        REQUIRE("55" == TimePointToMinutesString(event.date_till));
        REQUIRE(returnString == "1:05 " + utils::localize.get(utils::time::Locale::getAM()) + " - 7:55 " +
                                    utils::localize.get(utils::time::Locale::getPM()));
    }

    SECTION("Before noon time input - short version (mode12h)")
    {
        auto event      = EventsRecord();
        event.date_from = TimePointFromString("2020-12-31 00:05:00");
        event.date_till = TimePointFromString("2020-12-31 19:55:00");

        auto returnString = utils::time::TimeRangeParser().getCalendarTimeString(
            event.date_from, event.date_till, Version::Abbrev, false);
        REQUIRE("2020-12-31 00:05:00" == TimePointToString(event.date_from));
        REQUIRE("2020-12-31 19:55:00" == TimePointToString(event.date_till));
        REQUIRE("12" == TimePointToHourString12H(event.date_from));
        REQUIRE("05" == TimePointToMinutesString(event.date_from));
        REQUIRE(returnString == "12:05 " + utils::localize.get(utils::time::Locale::getAM()));
    }

    SECTION("After noon time input - short version (mode12h)")
    {
        auto event      = EventsRecord();
        event.date_from = TimePointFromString("2020-12-31 12:05:00");
        event.date_till = TimePointFromString("2020-12-31 19:55:00");

        auto returnString = utils::time::TimeRangeParser().getCalendarTimeString(
            event.date_from, event.date_till, Version::Abbrev, false);
        REQUIRE("2020-12-31 12:05:00" == TimePointToString(event.date_from));
        REQUIRE("2020-12-31 19:55:00" == TimePointToString(event.date_till));
        REQUIRE("12" == TimePointToHourString12H(event.date_from));
        REQUIRE("05" == TimePointToMinutesString(event.date_from));
        REQUIRE(returnString == "12:05 " + utils::localize.get(utils::time::Locale::getPM()));
    }

    SECTION("Time input (mode24h)")
    {
        auto event      = EventsRecord();
        event.date_from = TimePointFromString("2021-01-01 01:05:00");
        event.date_till = TimePointFromString("2021-01-01 19:55:00");

        auto returnString = utils::time::TimeRangeParser().getCalendarTimeString(
            event.date_from, event.date_till, Version::DayEvent, true);
        REQUIRE("2021-01-01 01:05:00" == TimePointToString(event.date_from));
        REQUIRE("2021-01-01 19:55:00" == TimePointToString(event.date_till));
        REQUIRE("1" == TimePointToHourString24H(event.date_from));
        REQUIRE("05" == TimePointToMinutesString(event.date_from));
        REQUIRE("19" == TimePointToHourString24H(event.date_till));
        REQUIRE("55" == TimePointToMinutesString(event.date_till));
        REQUIRE(returnString == "1:05 - 19:55");
    }

    SECTION("Time input - short version (mode24h)")
    {
        auto event      = EventsRecord();
        event.date_from = TimePointFromString("2021-01-01 18:05:00");
        event.date_till = TimePointFromString("2021-01-01 19:55:00");

        auto returnString = utils::time::TimeRangeParser().getCalendarTimeString(
            event.date_from, event.date_till, Version::Abbrev, true);
        REQUIRE("2021-01-01 18:05:00" == TimePointToString(event.date_from));
        REQUIRE("2021-01-01 19:55:00" == TimePointToString(event.date_till));
        REQUIRE("18" == TimePointToHourString24H(event.date_from));
        REQUIRE("05" == TimePointToMinutesString(event.date_from));
        REQUIRE(returnString == "18:05");
    }

    SECTION("All day time input")
    {
        auto event      = EventsRecord();
        event.date_from = TimePointFromString("2020-10-20 00:00:00");
        event.date_till = TimePointFromString("2020-10-20 23:59:00");

        auto returnString = utils::time::TimeRangeParser().getCalendarTimeString(
            event.date_from, event.date_till, Version::DayEvent, false);
        REQUIRE("2020-10-20 00:00:00" == TimePointToString(event.date_from));
        REQUIRE("2020-10-20 23:59:00" == TimePointToString(event.date_till));
        REQUIRE(returnString == utils::localize.get("app_calendar_all_day"));
    }
}
