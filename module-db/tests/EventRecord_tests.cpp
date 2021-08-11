// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common.hpp"

#include <Interface/EventRecord.hpp>

#include <time/dateCommon.hpp>

#include <catch2/catch.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>

TEST_CASE("EventRecord tests")
{
    SECTION("Constructor - lastOccurrence set")
    {
        auto er = EventRecord(1,
                              "TestAlarmName1",
                              TimePointFromString("2020-01-11 12:00:00"),
                              TimePointFromString("2020-01-11 13:00:00"),
                              60,
                              false,
                              "");
        REQUIRE(er.endDate == TimePointFromString("2020-01-11 13:00:00"));
    }
    SECTION("Constructor - single event")
    {
        auto er = EventRecord(1,
                              "TestAlarmName1",
                              TimePointFromString("2020-01-11 12:00:00"),
                              TimePointFromString("2020-01-11 13:00:00"),
                              60,
                              false,
                              "");
        REQUIRE(er.endDate == TimePointFromString("2020-01-11 13:00:00"));
    }
    SECTION("Constructor - recurring forever")
    {
        auto er = EventRecord(1,
                              "TestAlarmName1",
                              TimePointFromString("2020-01-11 12:00:00"),
                              TimePointFromString("2020-01-11 13:00:00"),
                              60,
                              false,
                              "FREQ=DAILY");
        REQUIRE(er.endDate == TIME_POINT_MAX);
    }
    SECTION("Constructor - recurring count")
    {
        auto er = EventRecord(1,
                              "TestAlarmName1",
                              TimePointFromString("2020-01-11 12:00:00"),
                              TimePointFromString("2020-01-11 13:00:00"),
                              60,
                              false,
                              "FREQ=DAILY;COUNT=5");
        REQUIRE(er.endDate == TimePointFromString("2020-01-15 13:00:00"));
    }
    SECTION("Constructor - recurring until")
    {
        auto er = EventRecord(1,
                              "TestAlarmName1",
                              TimePointFromString("2020-01-11 12:00:00"),
                              TimePointFromString("2020-01-11 13:00:00"),
                              60,
                              false,
                              "FREQ=DAILY;UNTIL=20200115T150000Z");
        REQUIRE(er.endDate == TimePointFromString("2020-01-15 13:00:00"));
    }

    SECTION("Generate events in range basic")
    {
        auto er = EventRecord(1,
                              "TestAlarmName1",
                              TimePointFromString("2020-01-11 12:00:00"),
                              TimePointFromString("2020-01-11 13:00:00"),
                              60,
                              false,
                              "FREQ=DAILY");

        auto events = er.generateSingleEvents(
            TimePointFromString("2020-01-15 00:00:01"), TimePointFromString("2020-01-20 00:00:01"), 100);
        REQUIRE(events.size() == 5);
        REQUIRE(events[0].startDate == TimePointFromString("2020-01-15 12:00:00"));
        REQUIRE(events[0].endDate == TimePointFromString("2020-01-15 13:00:00"));
        REQUIRE(events[4].startDate == TimePointFromString("2020-01-19 12:00:00"));
        REQUIRE(events[4].endDate == TimePointFromString("2020-01-19 13:00:00"));
    }

    SECTION("Generate events in range before event")
    {
        auto er = EventRecord(1,
                              "TestAlarmName1",
                              TimePointFromString("2020-01-11 12:00:00"),
                              TimePointFromString("2020-01-11 13:00:00"),
                              60,
                              false,
                              "FREQ=DAILY");

        auto events = er.generateSingleEvents(
            TimePointFromString("2020-01-01 00:00:00"), TimePointFromString("2020-01-20 00:00:00"), 100);
        REQUIRE(events.size() == 9);
        REQUIRE(events[0].startDate == TimePointFromString("2020-01-11 12:00:00"));
        REQUIRE(events[0].endDate == TimePointFromString("2020-01-11 13:00:00"));
        REQUIRE(events[8].startDate == TimePointFromString("2020-01-19 12:00:00"));
        REQUIRE(events[8].endDate == TimePointFromString("2020-01-19 13:00:00"));
    }

    SECTION("Generate events in range before empty")
    {
        auto er = EventRecord(1,
                              "TestAlarmName1",
                              TimePointFromString("2020-01-11 12:00:00"),
                              TimePointFromString("2020-01-11 13:00:00"),
                              60,
                              false,
                              "FREQ=DAILY");

        auto events = er.generateSingleEvents(
            TimePointFromString("2020-01-01 00:00:00"), TimePointFromString("2020-01-10 00:00:00"), 100);
        REQUIRE(events.size() == 0);
    }
}
