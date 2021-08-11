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
    Database::initialize();

    const auto eventsPath = (std::filesystem::path{"sys/user"} / "events.db");
    RemoveDbFiles(eventsPath.stem());

    auto eventsDB = EventsDB(eventsPath.c_str());
    REQUIRE(eventsDB.isInitialized());

    AlarmEventRecordInterface alarmEventRecordInterface(&eventsDB);

    auto getQuery = [&](uint32_t id,
                        UTF8 name,
                        TimePoint startDate,
                        TimePoint endDate,
                        uint32_t duration,
                        bool isAllDay,
                        std::string rrule,
                        std::string musicTone,
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
        REQUIRE(alarmRec.rrule == rrule);
        REQUIRE(alarmRec.musicTone == musicTone);
        REQUIRE(alarmRec.enabled == enabled);
        REQUIRE(alarmRec.snoozeDuration == snoozeDuration);

        return alarmRec;
    };

    auto addQuery = [&](UTF8 name,
                        TimePoint startDate,
                        TimePoint endDate,
                        uint32_t duration,
                        bool isAllDay,
                        std::string rrule,
                        std::string musicTone,
                        bool enabled,
                        uint32_t snoozeDuration) {
        auto alarmEvent = AlarmEventRecord(
            0, name, startDate, endDate, duration, isAllDay, rrule, musicTone, enabled, snoozeDuration);
        auto query  = std::make_shared<db::query::alarmEvents::Add>(alarmEvent);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::AddResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->succeed());
    };

    auto getLimitedQuery = [&](uint32_t offset, uint32_t limit) {
        auto query  = std::make_shared<db::query::alarmEvents::GetLimited>(offset, limit);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::GetLimitedResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmsRec = result->getResult();
        return alarmsRec;
    };

    auto GetBetweenDatesQuery = [&](TimePoint start, TimePoint end, uint32_t offset, uint32_t limit) {
        auto query  = std::make_shared<db::query::alarmEvents::GetBetweenDates>(start, end, offset, limit);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::GetBetweenDatesResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmsRec = result->getResult();
        return alarmsRec;
    };

    auto GetRecurringBetweenDatesQuery = [&](TimePoint start, TimePoint end, uint32_t offset, uint32_t limit) {
        auto query  = std::make_shared<db::query::alarmEvents::GetRecurringBetweenDates>(start, end, offset, limit);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::GetRecurringBetweenDatesResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmsRec = result->getResult();
        return alarmsRec;
    };

    auto updateQuery = [&](AlarmEventRecord alarmEvent) {
        auto query  = std::make_shared<db::query::alarmEvents::Edit>(alarmEvent);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::EditResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->succeed());
    };

    auto getNextQuery = [&](TimePoint start, uint32_t offset, uint32_t limit) {
        auto query  = std::make_shared<db::query::alarmEvents::GetNext>(start, offset, limit);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::GetNextResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmsRec = result->getResult();
        return alarmsRec;
    };

    SECTION("Add & Get")
    {
        addQuery("TestAlarmName1",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "",
                 "music.wav",
                 true,
                 5);
        addQuery("TestAlarmName2",
                 TimePointFromString("2020-06-11 19:00:00"),
                 TimePointFromString("2020-06-11 20:00:00"),
                 60,
                 true,
                 "",
                 "music.wav",
                 false,
                 10);
        addQuery("TestAlarmName3",
                 TimePointFromString("2020-09-11 23:00:00"),
                 TimePointFromString("2020-09-11 23:30:00"),
                 30,
                 false,
                 "",
                 "music.wav",
                 true,
                 15);

        getQuery(1,
                 "TestAlarmName1",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "",
                 "music.wav",
                 true,
                 5);
        getQuery(2,
                 "TestAlarmName2",
                 TimePointFromString("2020-06-11 19:00:00"),
                 TimePointFromString("2020-06-11 20:00:00"),
                 60,
                 true,
                 "",
                 "music.wav",
                 false,
                 10);
        getQuery(3,
                 "TestAlarmName3",
                 TimePointFromString("2020-09-11 23:00:00"),
                 TimePointFromString("2020-09-11 23:30:00"),
                 30,
                 false,
                 "",
                 "music.wav",
                 true,
                 15);
    }

    SECTION("Add & Get Recurring")
    {
        addQuery("TestAlarmName1",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "FREQ=DAILY",
                 "music.wav",
                 true,
                 5);
        addQuery("TestAlarmName2",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "FREQ=DAILY;COUNT=5",
                 "music.wav",
                 true,
                 5);
        addQuery("TestAlarmName3",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "FREQ=DAILY;UNTIL=20200115T150000Z",
                 "music.wav",
                 true,
                 5);
        getQuery(1,
                 "TestAlarmName1",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TIME_POINT_MAX,
                 60,
                 false,
                 "FREQ=DAILY",
                 "music.wav",
                 true,
                 5);
        getQuery(2,
                 "TestAlarmName2",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-15 13:00:00"),
                 60,
                 false,
                 "FREQ=DAILY;COUNT=5",
                 "music.wav",
                 true,
                 5);
        getQuery(3,
                 "TestAlarmName3",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-15 13:00:00"),
                 60,
                 false,
                 "FREQ=DAILY;UNTIL=20200115T150000Z",
                 "music.wav",
                 true,
                 5);
    }

    SECTION("Add, Remove, Get nonexisting")
    {
        addQuery("TestAlarmName",
                 TimePointFromString("2020-11-11 09:00:00"),
                 TimePointFromString("2020-11-11 10:00:00"),
                 60,
                 false,
                 "",
                 "music.wav",
                 true,
                 15);

        auto queryRemove  = std::make_shared<db::query::alarmEvents::Remove>(1);
        auto retRemove    = alarmEventRecordInterface.runQuery(queryRemove);
        auto resultRemove = dynamic_cast<db::query::alarmEvents::RemoveResult *>(retRemove.get());
        REQUIRE(resultRemove != nullptr);
        REQUIRE(resultRemove->succeed());

        auto queryGet  = std::make_shared<db::query::alarmEvents::Get>(1);
        auto retGet    = alarmEventRecordInterface.runQuery(queryGet);
        auto resultGet = dynamic_cast<db::query::alarmEvents::GetResult *>(retGet.get());
        auto alarmRec  = resultGet->getResult();
        REQUIRE(resultGet != nullptr);
        REQUIRE(alarmRec.ID == 0);
    }

    SECTION("Add and Update")
    {
        addQuery("TestAlarmName",
                 TimePointFromString("2020-11-11 09:00:00"),
                 TimePointFromString("2020-11-11 10:00:00"),
                 60,
                 false,
                 "",
                 "music.wav",
                 true,
                 15);
        auto alarmEvent = getQuery(1,
                                   "TestAlarmName",
                                   TimePointFromString("2020-11-11 09:00:00"),
                                   TimePointFromString("2020-11-11 10:00:00"),
                                   60,
                                   false,
                                   "",
                                   "music.wav",
                                   true,
                                   15);

        alarmEvent.name      = "NewName";
        alarmEvent.musicTone = "NewMusic.wav";
        updateQuery(alarmEvent);

        getQuery(1,
                 "NewName",
                 TimePointFromString("2020-11-11 09:00:00"),
                 TimePointFromString("2020-11-11 10:00:00"),
                 60,
                 false,
                 "",
                 "NewMusic.wav",
                 true,
                 15);
    }

    SECTION("Get limit offset")
    {
        addQuery("TestAlarmName1",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "",
                 "music.wav",
                 true,
                 5);
        addQuery("TestAlarmName2",
                 TimePointFromString("2020-06-11 19:00:00"),
                 TimePointFromString("2020-06-11 20:00:00"),
                 60,
                 true,
                 "",
                 "music.wav",
                 false,
                 10);
        addQuery("TestAlarmName3",
                 TimePointFromString("2020-09-11 23:00:00"),
                 TimePointFromString("2020-09-11 23:30:00"),
                 30,
                 false,
                 "",
                 "music.wav",
                 true,
                 15);

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
        addQuery("TestAlarmName1",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "",
                 "music.wav",
                 true,
                 5);
        addQuery("TestAlarmName2",
                 TimePointFromString("2020-06-11 19:00:00"),
                 TimePointFromString("2020-06-11 20:00:00"),
                 60,
                 true,
                 "",
                 "music.wav",
                 false,
                 10);
        addQuery("TestAlarmName3",
                 TimePointFromString("2020-09-11 23:00:00"),
                 TimePointFromString("2020-09-11 23:30:00"),
                 30,
                 false,
                 "",
                 "music.wav",
                 true,
                 15);

        auto retAlarmEvents = GetBetweenDatesQuery(
            TimePointFromString("2020-01-01 00:00:00"), TimePointFromString("2020-02-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.size() == 1);
        REQUIRE(retAlarmEvents[0].ID == 1);

        retAlarmEvents = GetBetweenDatesQuery(
            TimePointFromString("2020-05-01 00:00:00"), TimePointFromString("2020-10-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.size() == 2);
        REQUIRE(retAlarmEvents[0].ID == 2);
        REQUIRE(retAlarmEvents[1].ID == 3);

        retAlarmEvents = GetBetweenDatesQuery(
            TimePointFromString("2020-05-01 00:00:00"), TimePointFromString("2020-10-01 00:00:00"), 1, 10);
        REQUIRE(retAlarmEvents.size() == 1);
        REQUIRE(retAlarmEvents[0].ID == 3);
    }

    SECTION("Get Next")
    {
        addQuery("TestAlarmName1",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "",
                 "music.wav",
                 true,
                 5);
        addQuery("TestAlarmName2",
                 TimePointFromString("2020-01-11 12:30:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 30,
                 false,
                 "",
                 "music.wav",
                 true,
                 5);

        auto retAlarmEvents = getNextQuery(TimePointFromString("2020-01-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.size() == 1);
        REQUIRE(retAlarmEvents[0].ID == 1);

        addQuery("TestAlarmName3",
                 TimePointFromString("2020-01-10 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "",
                 "music.wav",
                 true,
                 5);
        retAlarmEvents = getNextQuery(TimePointFromString("2020-01-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.size() == 1);
        REQUIRE(retAlarmEvents[0].ID == 3);

        addQuery("TestAlarmName4",
                 TimePointFromString("2020-01-10 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "",
                 "music.wav",
                 true,
                 5);

        retAlarmEvents = GetBetweenDatesQuery(
            TimePointFromString("2020-05-01 00:00:00"), TimePointFromString("2020-10-01 00:00:00"), 1, 10);
        retAlarmEvents = getNextQuery(TimePointFromString("2020-01-01 00:00:00"), 0, 10);
        REQUIRE(retAlarmEvents.size() == 2);
        REQUIRE((retAlarmEvents[0].ID + retAlarmEvents[1].ID == 7));
    }

    SECTION("Get Recurring Between Dates")
    {
        addQuery("TestAlarmName1",
                 TimePointFromString("2020-01-11 12:00:00"),
                 TimePointFromString("2020-01-11 13:00:00"),
                 60,
                 false,
                 "FREQ=DAILY",
                 "music.wav",
                 true,
                 5);
        addQuery("TestAlarmName2",
                 TimePointFromString("2020-01-11 15:00:00"),
                 TimePointFromString("2020-01-11 16:00:00"),
                 60,
                 false,
                 "FREQ=DAILY;COUNT=5",
                 "music.wav",
                 true,
                 5);
        addQuery("TestAlarmName3",
                 TimePointFromString("2020-01-11 17:00:00"),
                 TimePointFromString("2020-01-11 18:00:00"),
                 60,
                 false,
                 "FREQ=DAILY;UNTIL=20200115T170000Z",
                 "music.wav",
                 true,
                 5);
        auto alarms = GetRecurringBetweenDatesQuery(
            TimePointFromString("2020-01-01 12:00:00"), TimePointFromString("2020-01-11 11:00:00"), 0, 100);
        REQUIRE(alarms.size() == 0);
        alarms = GetRecurringBetweenDatesQuery(
            TimePointFromString("2020-01-01 12:00:00"), TimePointFromString("2020-01-11 12:00:00"), 0, 100);
        REQUIRE(alarms.size() == 1);
        alarms = GetRecurringBetweenDatesQuery(
            TimePointFromString("2020-01-01 12:00:00"), TimePointFromString("2020-01-11 17:00:00"), 0, 100);
        REQUIRE(alarms.size() == 3);
        alarms = GetRecurringBetweenDatesQuery(
            TimePointFromString("2020-02-01 12:00:00"), TimePointFromString("2020-02-11 13:00:00"), 0, 100);
        REQUIRE(alarms.size() == 1);
        REQUIRE(alarms[0].ID == 1);
        alarms = GetRecurringBetweenDatesQuery(
            TimePointFromString("2020-01-14 12:00:00"), TimePointFromString("2020-01-14 12:00:01"), 0, 100);
        REQUIRE(alarms.size() == 3);
        alarms = GetRecurringBetweenDatesQuery(
            TimePointFromString("2020-01-15 17:30:00"), TimePointFromString("2020-01-15 17:30:01"), 0, 100);
        REQUIRE(alarms.size() == 2);
    }

    Database::deinitialize();
}
