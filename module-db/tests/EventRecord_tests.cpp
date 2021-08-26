// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common.hpp"

#include <Interface/AlarmEventRecord.hpp>
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
    constexpr auto testName     = "TestAlarmName1";
    constexpr auto testDuration = 60;
    constexpr auto testIsAllDay = false;
    const auto testEventStart   = TimePointFromString("2020-01-11 12:00:00");

    SECTION("Constructor - single event")
    {
        EventRecord eventRecord(1, testName, testEventStart, testDuration, testIsAllDay, "");
        REQUIRE(eventRecord.endDate == TimePointFromString("2020-01-11 13:00:00"));
        REQUIRE(eventRecord.isValid());
    }
    SECTION("Constructor - recurring forever")
    {
        EventRecord eventRecord(1, testName, testEventStart, testDuration, testIsAllDay, "FREQ=DAILY");
        REQUIRE(eventRecord.endDate == TIME_POINT_MAX);
        REQUIRE(eventRecord.isValid());
    }
    SECTION("Constructor - recurring count")
    {
        EventRecord eventRecord(1, testName, testEventStart, testDuration, testIsAllDay, "FREQ=DAILY;COUNT=5");
        REQUIRE(eventRecord.endDate == TimePointFromString("2020-01-15 13:00:00"));
        REQUIRE(eventRecord.isValid());
    }
    SECTION("Constructor - recurring until")
    {
        EventRecord eventRecord(
            1, testName, testEventStart, testDuration, testIsAllDay, "FREQ=DAILY;UNTIL=20200115T150000Z");
        REQUIRE(eventRecord.endDate == TimePointFromString("2020-01-15 13:00:00"));
        REQUIRE(eventRecord.isValid());
    }

    SECTION("Generate events in range basic")
    {
        EventRecord eventRecord(1, testName, testEventStart, testDuration, testIsAllDay, "FREQ=DAILY");

        auto events = eventRecord.generateSingleEvents(
            TimePointFromString("2020-01-15 00:00:01"), TimePointFromString("2020-01-20 00:00:01"), 100);
        REQUIRE(events.size() == 5);
        REQUIRE(events[0].startDate == TimePointFromString("2020-01-15 12:00:00"));
        REQUIRE(events[0].endDate == TimePointFromString("2020-01-15 13:00:00"));
        REQUIRE(events[4].startDate == TimePointFromString("2020-01-19 12:00:00"));
        REQUIRE(events[4].endDate == TimePointFromString("2020-01-19 13:00:00"));
    }

    SECTION("Generate events in range before event")
    {
        EventRecord eventRecord(1, testName, testEventStart, testDuration, testIsAllDay, "FREQ=DAILY");

        auto events = eventRecord.generateSingleEvents(
            TimePointFromString("2020-01-01 00:00:00"), TimePointFromString("2020-01-20 00:00:00"), 100);
        REQUIRE(events.size() == 9);
        REQUIRE(events[0].startDate == testEventStart);
        REQUIRE(events[0].endDate == TimePointFromString("2020-01-11 13:00:00"));
        REQUIRE(events[8].startDate == TimePointFromString("2020-01-19 12:00:00"));
        REQUIRE(events[8].endDate == TimePointFromString("2020-01-19 13:00:00"));
    }

    SECTION("Generate events in range before empty")
    {
        EventRecord eventRecord(1, testName, testEventStart, testDuration, testIsAllDay, "FREQ=DAILY");

        auto events = eventRecord.generateSingleEvents(
            TimePointFromString("2020-01-01 00:00:00"), TimePointFromString("2020-01-10 00:00:00"), 100);
        REQUIRE(events.empty());
    }

    SECTION("Generate next event daily")
    {
        EventRecord eventRecord(1, testName, testEventStart, testDuration, testIsAllDay, "FREQ=DAILY");

        auto event = eventRecord.getNextSingleEvent(TimePointFromString("2000-01-01 00:00:00"));
        REQUIRE(event.name == eventRecord.name);
        REQUIRE(event.startDate == eventRecord.startDate);
        REQUIRE(event.endDate == TimePointFromString("2020-01-11 13:00:00"));
        REQUIRE(event.duration == eventRecord.duration);
        REQUIRE(event.isAllDay == eventRecord.isAllDay);
        REQUIRE(event.parent != nullptr);
        REQUIRE(event.parent->ID == eventRecord.ID);

        event = eventRecord.getNextSingleEvent(testEventStart);
        REQUIRE(event.startDate == eventRecord.startDate);
        REQUIRE(event.endDate == TimePointFromString("2020-01-11 13:00:00"));

        event = eventRecord.getNextSingleEvent(TimePointFromString("2020-02-27 17:00:00"));
        REQUIRE(event.startDate == TimePointFromString("2020-02-28 12:00:00"));
        REQUIRE(event.endDate == TimePointFromString("2020-02-28 13:00:00"));
    }

    SECTION("Generate next event daily - leap year")
    {
        EventRecord eventRecord(1, testName, testEventStart, testDuration, testIsAllDay, "FREQ=DAILY");

        auto event = eventRecord.getNextSingleEvent(TimePointFromString("2020-02-28 17:00:00"));
        REQUIRE(event.startDate == TimePointFromString("2020-02-29 12:00:00"));
        REQUIRE(event.endDate == TimePointFromString("2020-02-29 13:00:00"));

        event = eventRecord.getNextSingleEvent(TimePointFromString("2021-02-28 17:00:00"));
        REQUIRE(event.startDate == TimePointFromString("2021-03-01 12:00:00"));
        REQUIRE(event.endDate == TimePointFromString("2021-03-01 13:00:00"));
    }

    SECTION("Generate next AlarmEvent daily")
    {
        AlarmEventRecord eventRecord(
            1, testName, testEventStart, testDuration, testIsAllDay, "FREQ=DAILY", "", true, 15);

        auto event = eventRecord.getNextSingleEvent(TimePointFromString("2000-01-01 00:00:00"));
        REQUIRE(event.name == eventRecord.name);
        REQUIRE(event.startDate == eventRecord.startDate);
        REQUIRE(event.endDate == TimePointFromString("2020-01-11 13:00:00"));
        REQUIRE(event.duration == eventRecord.duration);
        REQUIRE(event.isAllDay == eventRecord.isAllDay);
        REQUIRE(event.parent != nullptr);
        auto parent = std::dynamic_pointer_cast<AlarmEventRecord>(event.parent);
        REQUIRE(parent->ID == eventRecord.ID);
        REQUIRE(parent->musicTone == eventRecord.musicTone);
        REQUIRE(parent->enabled == eventRecord.enabled);
        REQUIRE(parent->snoozeDuration == eventRecord.snoozeDuration);
    }
}
