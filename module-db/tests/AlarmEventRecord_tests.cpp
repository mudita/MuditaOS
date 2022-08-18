// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common.hpp"

#include <Database/Database.hpp>
#include <Databases/EventsDB.hpp>
#include <Interface/AlarmEventRecord.hpp>
#include <queries/alarm_events/QueryAlarmEventsAdd.hpp>
#include <queries/alarm_events/QueryAlarmEventsEdit.hpp>
#include <queries/alarm_events/QueryAlarmEventsGet.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetEnabled.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetInRange.hpp>
#include <queries/alarm_events/QueryAlarmEventsRemove.hpp>
#include <queries/alarm_events/QueryAlarmEventsToggleAll.hpp>
#include <catch2/catch.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>

using namespace std::chrono_literals;

TEST_CASE("AlarmEventRecord queries tests")
{
    constexpr auto testAlarmHour      = 12h;
    constexpr auto testAlarmMinute    = 0min;
    constexpr auto testEmptyRRuleText = "";
    constexpr auto testRRuleFiveDays  = "FREQ=DAILY;COUNT=5";
    constexpr auto testRRuleDaily     = "FREQ=DAILY";
    constexpr auto testMusicTone      = "music.wav";
    constexpr auto testEnabled        = true;
    constexpr auto testSnoozeDuration = 15;

    Database::initialize();

    const auto eventsPath = (std::filesystem::path{"sys/user"} / "events.db");
    RemoveDbFiles(eventsPath.stem());

    auto eventsDB = EventsDB(eventsPath.c_str());
    REQUIRE(eventsDB.isInitialized());

    AlarmEventRecordInterface alarmEventRecordInterface(&eventsDB);

    auto getQuery = [&](uint32_t id,
                        std::chrono::hours hour,
                        std::chrono::minutes minute,
                        const std::string &musicTone,
                        bool enabled,
                        uint32_t snoozeDuration,
                        const std::string &rruleText) {
        auto query  = std::make_shared<db::query::alarmEvents::Get>(id);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::GetResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmRec = result->getResult();
        REQUIRE(alarmRec.ID == id);
        REQUIRE(alarmRec.alarmTime.hourOfDay.count() == hour.count());
        REQUIRE(alarmRec.alarmTime.minuteOfHour.count() == minute.count());
        REQUIRE(alarmRec.rruleText == rruleText);
        REQUIRE(alarmRec.musicTone == musicTone);
        REQUIRE(alarmRec.enabled == enabled);
        REQUIRE(alarmRec.snoozeDuration == snoozeDuration);

        return alarmRec;
    };

    auto addQuery = [&](std::chrono::hours hour,
                        std::chrono::minutes minute,
                        const std::string &musicTone,
                        bool enabled,
                        uint32_t snoozeDuration,
                        const std::string &rruleText) {
        auto alarmEvent = AlarmEventRecord(0, AlarmTime{hour, minute}, musicTone, enabled, snoozeDuration, rruleText);
        auto query      = std::make_shared<db::query::alarmEvents::Add>(alarmEvent);
        auto ret        = alarmEventRecordInterface.runQuery(query);
        auto result     = dynamic_cast<db::query::alarmEvents::AddResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
    };

    auto getInRangeQuery = [&](const uint32_t offset, const uint32_t limit) {
        auto query  = std::make_shared<db::query::alarmEvents::GetInRange>(offset, limit);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::GetInRangeResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmsRec = result->getResult().first;
        return alarmsRec;
    };

    auto getEnabledQuery = [&]() {
        const auto query  = std::make_shared<db::query::alarmEvents::GetEnabled>();
        const auto ret    = alarmEventRecordInterface.runQuery(query);
        const auto result = dynamic_cast<db::query::alarmEvents::GetEnabledResult *>(ret.get());
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

    auto toggleAll = [&](const bool toggle) {
        auto query  = std::make_shared<db::query::alarmEvents::ToggleAll>(toggle);
        auto ret    = alarmEventRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarmEvents::ToggleAllResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmsRec = result->getResult();
        return alarmsRec;
    };

    SECTION("Add remove add")
    {
        auto retAlarmEvents = getInRangeQuery(0, 10);
        REQUIRE(retAlarmEvents.size() == 0);

        addQuery(testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testRRuleDaily);

        retAlarmEvents = getInRangeQuery(0, 10);
        REQUIRE(retAlarmEvents.size() == 1);

        const auto queryRemove  = std::make_shared<db::query::alarmEvents::Remove>(1);
        const auto retRemove    = alarmEventRecordInterface.runQuery(queryRemove);
        const auto resultRemove = dynamic_cast<db::query::alarmEvents::RemoveResult *>(retRemove.get());
        REQUIRE(resultRemove != nullptr);
        REQUIRE(resultRemove->getResult());

        retAlarmEvents = getInRangeQuery(0, 10);
        REQUIRE(retAlarmEvents.size() == 0);

        addQuery(testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testRRuleDaily);

        retAlarmEvents = getInRangeQuery(0, 10);
        REQUIRE(retAlarmEvents.size() == 1);
    }

    SECTION("Add & Get")
    {
        addQuery(testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
        addQuery(19h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
        addQuery(23h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);

        getQuery(1, testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
        getQuery(2, 19h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
        getQuery(3, 23h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
    }

    SECTION("Add & Get Recurring")
    {
        addQuery(testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testRRuleDaily);
        addQuery(testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testRRuleFiveDays);
        addQuery(testAlarmHour,
                 testAlarmMinute,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration,
                 "FREQ=DAILY;UNTIL=20200115T150000Z");

        getQuery(1, testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testRRuleDaily);
        getQuery(2, testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testRRuleFiveDays);
        getQuery(3,
                 testAlarmHour,
                 testAlarmMinute,
                 testMusicTone,
                 testEnabled,
                 testSnoozeDuration,
                 "FREQ=DAILY;UNTIL=20200115T150000Z");
    }

    SECTION("Add, Remove, Get nonexisting")
    {
        addQuery(9h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);

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
        addQuery(9h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
        auto alarmEvent = getQuery(1, 9h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);

        alarmEvent.musicTone = "NewMusic.wav";
        updateQuery(alarmEvent);

        getQuery(1, 9h, 0min, alarmEvent.musicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
    }

    SECTION("Get limit offset")
    {
        addQuery(testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
        addQuery(19h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
        addQuery(23h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);

        auto retAlarmEvents = getInRangeQuery(0, 3);
        REQUIRE(retAlarmEvents.size() == 3);

        retAlarmEvents = getInRangeQuery(1, 3);
        REQUIRE(retAlarmEvents.size() == 2);
        REQUIRE(retAlarmEvents[0].ID == 2);
        REQUIRE(retAlarmEvents[1].ID == 3);

        retAlarmEvents = getInRangeQuery(2, 10);
        REQUIRE(retAlarmEvents.size() == 1);
        REQUIRE(retAlarmEvents[0].ID == 3);
    }

    SECTION("Get Enabled")
    {
        addQuery(testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
        addQuery(19h, 0min, testMusicTone, false, testSnoozeDuration, testEmptyRRuleText);
        addQuery(23h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);

        auto retAlarmEvents = getEnabledQuery();
        REQUIRE(retAlarmEvents.size() == 2);
        REQUIRE(retAlarmEvents[0].ID == 1);
        REQUIRE(retAlarmEvents[1].ID == 3);
    }

    SECTION("ToggleAll Alarms")
    {
        addQuery(testAlarmHour, testAlarmMinute, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);
        addQuery(19h, 0min, testMusicTone, false, testSnoozeDuration, testEmptyRRuleText);
        addQuery(23h, 0min, testMusicTone, testEnabled, testSnoozeDuration, testEmptyRRuleText);

        auto retAlarmEvents = getInRangeQuery(0, 3);
        REQUIRE(retAlarmEvents.size() == 3);
        REQUIRE(retAlarmEvents[0].enabled);
        REQUIRE(!retAlarmEvents[1].enabled);
        REQUIRE(retAlarmEvents[2].enabled);

        toggleAll(false);
        retAlarmEvents = getInRangeQuery(0, 3);
        REQUIRE(retAlarmEvents.size() == 3);
        REQUIRE(!retAlarmEvents[0].enabled);
        REQUIRE(!retAlarmEvents[1].enabled);
        REQUIRE(!retAlarmEvents[2].enabled);

        toggleAll(true);
        retAlarmEvents = getInRangeQuery(0, 3);
        REQUIRE(retAlarmEvents.size() == 3);
        REQUIRE(retAlarmEvents[0].enabled);
        REQUIRE(retAlarmEvents[1].enabled);
        REQUIRE(retAlarmEvents[2].enabled);
    }

    Database::deinitialize();
}

TEST_CASE("AlarmEventRecord recurrence generation tests")
{
    const auto testEventTime = AlarmTime{12h, 0min};
    const auto alarmTime     = TimePointFromString("2000-01-01 12:00:00");

    const auto fromTimeT  = std::chrono::system_clock::to_time_t(alarmTime);
    const auto fromLocal  = std::localtime(&fromTimeT);
    fromLocal->tm_hour    = 12;
    fromLocal->tm_min     = 0;
    auto time             = TimePointFloorMinutes(std::chrono::system_clock::from_time_t(std::mktime(fromLocal)));
    auto currentTimeShift = alarmTime - time;

    constexpr auto testTone    = "tone";
    constexpr auto testEnabled = true;
    constexpr auto testSnooze  = 60;

    SECTION("Generate next event daily")
    {
        AlarmEventRecord eventRecord(1, testEventTime, testTone, testEnabled, testSnooze, "FREQ=DAILY");

        auto event = eventRecord.getNextSingleEvent(TimePointFromString("2000-01-01 00:00:00"));
        REQUIRE(event.startDate == alarmTime - currentTimeShift);
        REQUIRE(event.parent != nullptr);
        REQUIRE(event.parent->ID == eventRecord.ID);

        event = eventRecord.getNextSingleEvent(time);
        REQUIRE(event.startDate == time + date::days{1});

        event = eventRecord.getNextSingleEvent(TimePointFromString("2020-02-27 17:00:00"));

        REQUIRE(event.startDate == TimePointFromString("2020-02-28 12:00:00") - currentTimeShift);
    }

    SECTION("Generate next event daily - leap year")
    {
        AlarmEventRecord eventRecord(1, testEventTime, testTone, testEnabled, testSnooze, "FREQ=DAILY");

        auto event = eventRecord.getNextSingleEvent(TimePointFromString("2020-02-28 17:00:00"));

        REQUIRE(event.startDate == TimePointFromString("2020-02-29 12:00:00") - currentTimeShift);
        REQUIRE(event.endDate == TIME_POINT_MAX);

        event = eventRecord.getNextSingleEvent(TimePointFromString("2021-02-28 17:00:00"));
        REQUIRE(event.startDate == TimePointFromString("2021-03-01 12:00:00") - currentTimeShift);
        REQUIRE(event.endDate == TIME_POINT_MAX);
    }

    SECTION("Generate next AlarmEvent daily")
    {
        AlarmEventRecord eventRecord(1, testEventTime, testTone, testEnabled, testSnooze, "FREQ=DAILY");

        auto event = eventRecord.getNextSingleEvent(TimePointFromString("2000-01-01 00:00:00"));
        REQUIRE(event.startDate == alarmTime - currentTimeShift);
        REQUIRE(event.endDate == TIME_POINT_MAX);
        REQUIRE(event.parent != nullptr);
        auto parent = std::dynamic_pointer_cast<AlarmEventRecord>(event.parent);
        REQUIRE(parent->ID == eventRecord.ID);
        REQUIRE(parent->musicTone == eventRecord.musicTone);
        REQUIRE(parent->enabled == eventRecord.enabled);
        REQUIRE(parent->snoozeDuration == eventRecord.snoozeDuration);
    }
}
