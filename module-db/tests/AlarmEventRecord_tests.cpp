// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common.hpp"

#include <Database/Database.hpp>
#include <Databases/EventsDB.hpp>
#include <Interface/AlarmEventRecord.hpp>
#include <queries/alarm_events/QueryAlarmEventsAdd.hpp>
#include <queries/alarm_events/QueryAlarmEventsEdit.hpp>
#include <queries/alarm_events/QueryAlarmEventsGet.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetLimited.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetBetweenDates.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetRecurringBetweenDates.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetNext.hpp>
#include <queries/alarm_events/QueryAlarmEventsRemove.hpp>
#include <catch2/catch.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>

TEST_CASE("AlarmEventRecord tests")
{
    constexpr auto testName1          = "TestAlarmName1";
    constexpr auto testName2          = "TestAlarmName2";
    constexpr auto testName3          = "TestAlarmName3";
    constexpr auto testDuration       = 60;
    constexpr auto testIsAllDay       = false;
    constexpr auto testEmptyRRuleText = "";
    constexpr auto testRRuleFiveDays  = "FREQ=DAILY;COUNT=5";
    constexpr auto testRRuleDaily     = "FREQ=DAILY";
    constexpr auto testMusicTone      = "music.wav";
    constexpr auto testEnabled        = true;
    constexpr auto testSnoozeDuration = 15;
    const auto testEventStart         = TimePointFromString("2020-01-11 12:00:00");

    Database::initialize();

    const auto eventsPath = (std::filesystem::path{"sys/user"} / "events.db");
    RemoveDbFiles(eventsPath.stem());

    auto eventsDB = EventsDB(eventsPath.c_str());
    REQUIRE(eventsDB.isInitialized());

    AlarmEventRecordInterface alarmEventRecordInterface(&eventsDB);

    auto getQuery = [&](uint32_t id,
                        const UTF8 &name,
                        TimePoint startDate,
                        TimePoint endDate,
                        uint32_t duration,
                        bool isAllDay,
                        const std::string &rruleText,
                        const std::string &musicTone,
                        bool enabled,
                        uint32_t snoozeDuration) {
        auto query  = std::make_shared<db::query::alarmEvents::Get>(id);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::GetResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmRec = result->getResult();
        REQUIRE(alarmRec.ID == id);
        REQUIRE(alarmRec.name == name);
        REQUIRE(alarmRec.startDate == startDate);
        REQUIRE(alarmRec.endDate == endDate);
        REQUIRE(alarmRec.duration == duration);
        REQUIRE(alarmRec.isAllDay == isAllDay);
        REQUIRE(alarmRec.rruleText == rruleText);
        REQUIRE(alarmRec.musicTone == musicTone);
        REQUIRE(alarmRec.enabled == enabled);
        REQUIRE(alarmRec.snoozeDuration == snoozeDuration);

        return alarmRec;
    };

    auto addQuery = [&](const UTF8 &name,
                        TimePoint startDate,
                        uint32_t duration,
                        bool isAllDay,
                        const std::string &rruleText,
                        const std::string &musicTone,
                        bool enabled,
                        uint32_t snoozeDuration) {
        auto alarmEvent =
            AlarmEventRecord(0, name, startDate, duration, isAllDay, rruleText, musicTone, enabled, snoozeDuration);
        auto query  = std::make_shared<db::query::alarmEvents::Add>(alarmEvent);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::AddResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
    };

    auto getLimitedQuery = [&](const uint32_t offset, const uint32_t limit) {
        auto query  = std::make_shared<db::query::alarmEvents::GetLimited>(offset, limit);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::GetLimitedResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmsRec = result->getResult();
        return alarmsRec;
    };

    auto getBetweenDatesQuery =
        [&](const TimePoint start, const TimePoint end, const uint32_t offset, const uint32_t limit) {
            const auto query  = std::make_shared<db::query::alarmEvents::GetBetweenDates>(start, end, offset, limit);
            const auto ret    = alarmEventRecordInterface.runQuery(query);
            const auto result = dynamic_cast<db::query::alarmEvents::GetBetweenDatesResult *>(ret.get());
            REQUIRE(result != nullptr);
            auto alarmsRec = result->getResult();
            return alarmsRec;
        };

    auto getRecurringBetweenDatesQuery = [&](TimePoint start, TimePoint end, uint32_t offset, uint32_t limit) {
        const auto query =
            std::make_shared<db::query::alarmEvents::GetRecurringBetweenDates>(start, end, offset, limit);
        const auto ret    = alarmEventRecordInterface.runQuery(query);
        const auto result = dynamic_cast<db::query::alarmEvents::GetRecurringBetweenDatesResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmsRec = result->getResult();
        return alarmsRec;
    };

    auto updateQuery = [&](const AlarmEventRecord &alarmEvent) {
        const auto query  = std::make_shared<db::query::alarmEvents::Edit>(alarmEvent);
        const auto ret    = alarmEventRecordInterface.runQuery(query);
        const auto result = dynamic_cast<db::query::alarmEvents::EditResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
    };

    auto getNextQuery = [&](const TimePoint start, const uint32_t offset, const uint32_t limit) {
        const auto query  = std::make_shared<db::query::alarmEvents::GetNext>(start, offset, limit);
        const auto ret    = alarmEventRecordInterface.runQuery(query);
        const auto result = dynamic_cast<db::query::alarmEvents::GetNextResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmsRec = result->getResult();
        return alarmsRec;
    };

    SECTION("Add remove add")
    {
        auto retAlarmEvents = getLimitedQuery(0, 10);
        REQUIRE(retAlarmEvents.size() == 0);

        addQuery(testName1,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testRRuleDaily,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);

        retAlarmEvents = getLimitedQuery(0, 10);
        REQUIRE(retAlarmEvents.size() == 1);

        const auto queryRemove  = std::make_shared<db::query::alarmEvents::Remove>(1);
        const auto retRemove    = alarmEventRecordInterface.runQuery(queryRemove);
        const auto resultRemove = dynamic_cast<db::query::alarmEvents::RemoveResult *>(retRemove.get());
        REQUIRE(resultRemove != nullptr);
        REQUIRE(resultRemove->getResult());

        retAlarmEvents = getLimitedQuery(0, 10);
        REQUIRE(retAlarmEvents.size() == 0);

        addQuery(testName1,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testRRuleDaily,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);

        retAlarmEvents = getLimitedQuery(0, 10);
        REQUIRE(retAlarmEvents.size() == 1);
    }

    SECTION("Add & Get")
    {
        addQuery(testName1,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        addQuery(testName2,
                 TimePointFromString("2020-06-11 19:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 false,
                 testSnoozeDuration);
        addQuery(testName3,
                 TimePointFromString("2020-09-11 23:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);

        getQuery(1,
                 testName1,
                 testEventStart,
                 TimePointFromString("2020-01-11 13:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        getQuery(2,
                 testName2,
                 TimePointFromString("2020-06-11 19:00:00"),
                 TimePointFromString("2020-06-11 20:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 false,
                 testSnoozeDuration);
        getQuery(3,
                 testName3,
                 TimePointFromString("2020-09-11 23:00:00"),
                 TimePointFromString("2020-09-12 00:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
    }

    SECTION("Add & Get Recurring")
    {
        addQuery(testName1,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testRRuleDaily,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        addQuery(testName2,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testRRuleFiveDays,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        addQuery(testName3,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 "FREQ=DAILY;UNTIL=20200115T150000Z",
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        getQuery(1,
                 testName1,
                 testEventStart,
                 TIME_POINT_MAX,
                 testDuration,
                 testIsAllDay,
                 testRRuleDaily,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        getQuery(2,
                 testName2,
                 testEventStart,
                 TimePointFromString("2020-01-15 13:00:00"),
                 testDuration,
                 testIsAllDay,
                 testRRuleFiveDays,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        getQuery(3,
                 testName3,
                 testEventStart,
                 TimePointFromString("2020-01-15 13:00:00"),
                 testDuration,
                 testIsAllDay,
                 "FREQ=DAILY;UNTIL=20200115T150000Z",
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
    }

    SECTION("Add, Remove, Get nonexisting")
    {
        addQuery(testName1,
                 TimePointFromString("2020-11-11 09:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);

        const auto queryRemove  = std::make_shared<db::query::alarmEvents::Remove>(1);
        const auto retRemove    = alarmEventRecordInterface.runQuery(queryRemove);
        const auto resultRemove = dynamic_cast<db::query::alarmEvents::RemoveResult *>(retRemove.get());
        REQUIRE(resultRemove != nullptr);
        REQUIRE(resultRemove->getResult());

        const auto queryGet  = std::make_shared<db::query::alarmEvents::Get>(1);
        const auto retGet    = alarmEventRecordInterface.runQuery(queryGet);
        const auto resultGet = dynamic_cast<db::query::alarmEvents::GetResult *>(retGet.get());
        const auto alarmRec  = resultGet->getResult();
        REQUIRE(resultGet != nullptr);
        REQUIRE(alarmRec.ID == 0);
    }

    SECTION("Add and Update")
    {
        addQuery("TestAlarmName",
                 TimePointFromString("2020-11-11 09:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        auto alarmEvent = getQuery(1,
                                   "TestAlarmName",
                                   TimePointFromString("2020-11-11 09:00:00"),
                                   TimePointFromString("2020-11-11 10:00:00"),
                                   testDuration,
                                   testIsAllDay,
                                   testEmptyRRuleText,
                                   testMusicTone,
                                   testEnabled,
                                   testSnoozeDuration);

        alarmEvent.name      = "NewName";
        alarmEvent.musicTone = "NewMusic.wav";
        updateQuery(alarmEvent);

        getQuery(1,
                 "NewName",
                 TimePointFromString("2020-11-11 09:00:00"),
                 TimePointFromString("2020-11-11 10:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 "NewMusic.wav",
                 testEnabled,
                 testSnoozeDuration);
    }

    SECTION("Get limit offset")
    {
        addQuery(testName1,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        addQuery(testName2,
                 TimePointFromString("2020-06-11 19:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 false,
                 testSnoozeDuration);
        addQuery(testName3,
                 TimePointFromString("2020-09-11 23:00:00"),
                 30,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);

        auto retAlarmEvents = getLimitedQuery(0, 3);
        REQUIRE(retAlarmEvents.size() == 3);

        retAlarmEvents = getLimitedQuery(1, 3);
        REQUIRE(retAlarmEvents.size() == 2);
        REQUIRE(retAlarmEvents[0].ID == 2);
        REQUIRE(retAlarmEvents[1].ID == 3);

        retAlarmEvents = getLimitedQuery(2, 10);
        REQUIRE(retAlarmEvents.size() == 1);
        REQUIRE(retAlarmEvents[0].ID == 3);
    }

    SECTION("Get limit offset between dates")
    {
        addQuery(testName1,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        addQuery(testName2,
                 TimePointFromString("2020-06-11 19:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 false,
                 testSnoozeDuration);
        addQuery(testName3,
                 TimePointFromString("2020-09-11 23:00:00"),
                 30,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);

        auto retAlarmEvents = getBetweenDatesQuery(
            TimePointFromString("2020-01-01 00:00:00"), TimePointFromString("2020-02-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.first.size() == 1);
        REQUIRE(retAlarmEvents.first[0].ID == 1);

        retAlarmEvents = getBetweenDatesQuery(
            TimePointFromString("2020-05-01 00:00:00"), TimePointFromString("2020-10-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.first.size() == 2);
        REQUIRE(retAlarmEvents.first[0].ID == 2);
        REQUIRE(retAlarmEvents.first[1].ID == 3);

        retAlarmEvents = getBetweenDatesQuery(
            TimePointFromString("2020-05-01 00:00:00"), TimePointFromString("2020-10-01 00:00:00"), 1, 10);
        REQUIRE(retAlarmEvents.first.size() == 1);
        REQUIRE(retAlarmEvents.first[0].ID == 3);

        REQUIRE(retAlarmEvents.second == 3);
    }

    SECTION("Get Next")
    {
        addQuery(testName1,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        addQuery(testName2,
                 TimePointFromString("2020-01-11 12:30:00"),
                 30,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);

        auto retAlarmEvents = getNextQuery(TimePointFromString("2020-01-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.size() == 1);
        REQUIRE(retAlarmEvents[0].ID == 1);

        addQuery(testName3,
                 TimePointFromString("2020-01-10 12:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        retAlarmEvents = getNextQuery(TimePointFromString("2020-01-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.size() == 1);
        REQUIRE(retAlarmEvents[0].ID == 3);

        addQuery("TestAlarmName4",
                 TimePointFromString("2020-01-10 12:00:00"),
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);

        retAlarmEvents =
            getBetweenDatesQuery(
                TimePointFromString("2020-05-01 00:00:00"), TimePointFromString("2020-10-01 00:00:00"), 1, 10)
                .first;
        retAlarmEvents = getNextQuery(TimePointFromString("2020-01-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.size() == 2);
        REQUIRE((((retAlarmEvents[0].ID == 3) && (retAlarmEvents[1].ID == 4)) ||
                 ((retAlarmEvents[0].ID == 4) && (retAlarmEvents[1].ID == 3))));
    }

    SECTION("Get Recurring Between Dates")
    {
        addQuery(testName1,
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testRRuleDaily,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        addQuery(testName2,
                 TimePointFromString("2020-01-11 15:00:00"),
                 testDuration,
                 testIsAllDay,
                 testRRuleFiveDays,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        addQuery(testName3,
                 TimePointFromString("2020-01-11 17:00:00"),
                 testDuration,
                 testIsAllDay,
                 "FREQ=DAILY;UNTIL=20200115T170000Z",
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        addQuery("TestAlarmName4",
                 testEventStart,
                 testDuration,
                 testIsAllDay,
                 testEmptyRRuleText,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration);
        auto alarms = getRecurringBetweenDatesQuery(
            TimePointFromString("2020-01-01 12:00:00"), TimePointFromString("2020-01-11 11:00:00"), 0, 100);
        REQUIRE(alarms.size() == 0);
        alarms = getRecurringBetweenDatesQuery(TimePointFromString("2020-01-01 12:00:00"), testEventStart, 0, 100);
        REQUIRE(alarms.size() == 1);
        alarms = getRecurringBetweenDatesQuery(
            TimePointFromString("2020-01-01 12:00:00"), TimePointFromString("2020-01-11 17:00:00"), 0, 100);
        REQUIRE(alarms.size() == 3);
        alarms = getRecurringBetweenDatesQuery(
            TimePointFromString("2020-02-01 12:00:00"), TimePointFromString("2020-02-11 13:00:00"), 0, 100);
        REQUIRE(alarms.size() == 1);
        REQUIRE(alarms[0].ID == 1);
        alarms = getRecurringBetweenDatesQuery(
            TimePointFromString("2020-01-14 12:00:00"), TimePointFromString("2020-01-14 12:00:01"), 0, 100);
        REQUIRE(alarms.size() == 3);
        alarms = getRecurringBetweenDatesQuery(
            TimePointFromString("2020-01-15 17:30:00"), TimePointFromString("2020-01-15 17:30:01"), 0, 100);
        REQUIRE(alarms.size() == 2);
    }

    Database::deinitialize();
}
