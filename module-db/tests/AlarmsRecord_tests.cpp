// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common.hpp"

#include "Database/Database.hpp"
#include "Databases/AlarmsDB.hpp"
#include "Interface/AlarmsRecord.hpp"
#include "queries/alarms/QueryAlarmsGet.hpp"
#include "queries/alarms/QueryAlarmsGetLimited.hpp"
#include "queries/alarms/QueryAlarmsRemove.hpp"
#include "queries/alarms/QueryAlarmsEdit.hpp"
#include "queries/alarms/QueryAlarmsAdd.hpp"
#include "queries/alarms/QueryAlarmsTurnOffAll.hpp"

#include <catch2/catch.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>

TEST_CASE("Alarms Record tests")
{
    Database::initialize();

    const auto alarmsPath = (std::filesystem::path{"sys/user"} / "alarms.db");
    RemoveDbFiles(alarmsPath.stem());

    AlarmsDB alarmsDB(alarmsPath.c_str());
    REQUIRE(alarmsDB.isInitialized());

    AlarmsRecordInterface alarmsRecordInterface(&alarmsDB);

    auto time = TimePointFromString("2020-11-11 15:15:00");

    auto getQuery = [&](uint32_t id,
                        TimePoint alarmTime,
                        uint32_t snooze,
                        AlarmStatus status,
                        uint32_t repeat,
                        const std::string &path) {
        auto query  = std::make_shared<db::query::alarms::Get>(id);
        auto ret    = alarmsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarms::GetResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto alarmRec = result->getResult();
        REQUIRE(alarmRec.ID == id);
        REQUIRE(alarmRec.time == alarmTime);
        REQUIRE(alarmRec.snooze == snooze);
        REQUIRE(alarmRec.status == status);
        REQUIRE(alarmRec.repeat == repeat);
        REQUIRE(alarmRec.path == path);

        return alarmRec;
    };

    auto recordsNumber = alarmsRecordInterface.GetCount();
    REQUIRE(alarmsDB.alarms.count() == 0);
    REQUIRE(recordsNumber == 0); // database should be empty after init

    // add initial record for tests
    AlarmsTableRow tableRow(1, time, 2, AlarmStatus::Off, 3, "musicFile.mp3");
    auto rec = AlarmsRecord(tableRow);
    REQUIRE(rec.time == time);
    REQUIRE(rec.snooze == 2);
    REQUIRE(rec.status == AlarmStatus::Off);
    REQUIRE(rec.repeat == 3);
    REQUIRE(rec.path == "musicFile.mp3");
    REQUIRE(rec.isValid());

    REQUIRE(alarmsRecordInterface.Add(rec));

    recordsNumber = alarmsRecordInterface.GetCount();
    REQUIRE(recordsNumber == 1);

    SECTION("Default Constructor")
    {
        AlarmsRecord test;
        REQUIRE(test.snooze == 0);
        REQUIRE(test.status == AlarmStatus::On);
        REQUIRE(test.repeat == 0);
        REQUIRE(test.path == "");
    }

    SECTION("Constructor from AlarmsTableRow")
    {
        AlarmsTableRow tableRow(1, time, 0, AlarmStatus::On, 0, "musicFile.mp3");
        AlarmsRecord test(tableRow);
        REQUIRE(test.time == time);
        REQUIRE(test.snooze == 0);
        REQUIRE(test.status == AlarmStatus::On);
        REQUIRE(test.repeat == 0);
        REQUIRE(test.path == "musicFile.mp3");
        REQUIRE(test.isValid());
    }

    SECTION("Get record by id with query")
    {
        getQuery(1, time, 2, AlarmStatus::Off, 3, "musicFile.mp3");
    }

    SECTION("Get entry by ID")
    {
        auto entry = alarmsRecordInterface.GetByID(1);
        REQUIRE(entry.ID == 1);
        REQUIRE(entry.time == time);
        REQUIRE(entry.snooze == 2);
        REQUIRE(entry.status == AlarmStatus::Off);
        REQUIRE(entry.repeat == 3);
        REQUIRE(entry.path == "musicFile.mp3");
        REQUIRE(entry.isValid());
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = alarmsRecordInterface.GetByID(100);
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.time == TIME_POINT_INVALID);
        REQUIRE(entry.snooze == 0);
        REQUIRE(entry.status == AlarmStatus::On);
        REQUIRE(entry.repeat == 0);
        REQUIRE(entry.path == "");
        REQUIRE_FALSE(entry.isValid());
    }

    AlarmsTableRow tableRow2(2, TimePointFromString("2020-11-12 18:00:00"), 3, AlarmStatus::On, 0, "musicFile123.mp3");
    auto rec2 = AlarmsRecord(tableRow2);
    REQUIRE(rec2.time == TimePointFromString("2020-11-12 18:00:00"));
    REQUIRE(rec2.snooze == 3);
    REQUIRE(rec2.status == AlarmStatus::On);
    REQUIRE(rec2.repeat == 0);
    REQUIRE(rec2.path == "musicFile123.mp3");
    REQUIRE(rec2.isValid());

    REQUIRE(alarmsRecordInterface.Add(rec2));

    recordsNumber = alarmsRecordInterface.GetCount();
    REQUIRE(recordsNumber == 2);

    SECTION("Entry update")
    {
        auto entryUpdate   = alarmsRecordInterface.GetByID(1);
        entryUpdate.time   = TimePointFromString("2020-12-31 23:59:00");
        entryUpdate.snooze = 0;
        entryUpdate.status = AlarmStatus::On;
        entryUpdate.path   = "musicFileUpdate.mp3";

        REQUIRE(alarmsRecordInterface.Update(entryUpdate));

        auto entry = alarmsRecordInterface.GetByID(1);
        REQUIRE(entry.ID == entryUpdate.ID);
        REQUIRE(entry.time == entryUpdate.time);
        REQUIRE(entry.snooze == entryUpdate.snooze);
        REQUIRE(entry.status == entryUpdate.status);
        REQUIRE(entry.repeat == entryUpdate.repeat);
        REQUIRE(entry.path == entryUpdate.path);
        REQUIRE(recordsNumber == 2);
    }

    SECTION("Get entries")
    {
        SECTION("Get table rows using valid offset/limit parameters")
        {
            auto retOffsetLimit = alarmsRecordInterface.GetLimitOffset(0, recordsNumber);
            REQUIRE(retOffsetLimit->size() == recordsNumber);
        }

        SECTION("Get table rows using limit which exceeds number of records in database")
        {
            auto retOffsetLimit = alarmsRecordInterface.GetLimitOffset(0, 100);
            REQUIRE(retOffsetLimit->size() == recordsNumber);
        }

        SECTION("Get table rows using invalid offset/limit parameters(should return empty object)")
        {
            auto retOffsetLimit = alarmsRecordInterface.GetLimitOffset(5, 4);
            REQUIRE(retOffsetLimit->empty());
        }
    }
    SECTION("Turn off all alarms")
    {
        REQUIRE(alarmsRecordInterface.TurnOffAll());
        auto retOffsetLimit = alarmsRecordInterface.GetLimitOffset(0, alarmsRecordInterface.GetCount());
        for (const auto &record : *retOffsetLimit) {
            REQUIRE(record.status == AlarmStatus::Off);
        }
    }

    SECTION("Turn off all alarms with query")
    {
        auto query  = std::make_shared<db::query::alarms::TurnOffAll>();
        auto ret    = alarmsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarms::TurnOffAllResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto retOffsetLimit = alarmsRecordInterface.GetLimitOffset(0, alarmsRecordInterface.GetCount());
        for (const auto &record : *retOffsetLimit) {
            REQUIRE(record.status == AlarmStatus::Off);
        }
    }

    SECTION("Remove entries")
    {
        REQUIRE(alarmsRecordInterface.RemoveByID(2));
        REQUIRE(alarmsRecordInterface.GetCount() == 1);

        REQUIRE_FALSE(alarmsRecordInterface.RemoveByID(100));

        REQUIRE(alarmsRecordInterface.RemoveByID(1));
        REQUIRE(alarmsRecordInterface.GetCount() == 0);
    }



    SECTION("Get all available records with query")
    {
        auto query  = std::make_shared<db::query::alarms::GetLimited>(0, UINT32_MAX);
        auto ret    = alarmsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarms::GetLimitedResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto results = result->getResult();
        REQUIRE(results.size() == 2);
    }

    SECTION("Get offset and limited records with query")
    {
        REQUIRE(alarmsRecordInterface.Add(rec2));
        REQUIRE(alarmsRecordInterface.Add(rec2));
        auto query  = std::make_shared<db::query::alarms::GetLimited>(1, 3);
        auto ret    = alarmsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarms::GetLimitedResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto results = result->getResult();
        REQUIRE(results.size() == 3);
        REQUIRE(result->getCountResult() == alarmsRecordInterface.GetCount());
    }

    SECTION("Remove records with query")
    {
        auto query1  = std::make_shared<db::query::alarms::Remove>(1);
        auto ret1    = alarmsRecordInterface.runQuery(query1);
        auto result1 = dynamic_cast<db::query::alarms::RemoveResult *>(ret1.get());
        REQUIRE(result1 != nullptr);
        REQUIRE(result1->succeed());

        auto query2  = std::make_shared<db::query::alarms::Remove>(2);
        auto ret2    = alarmsRecordInterface.runQuery(query2);
        auto result2 = dynamic_cast<db::query::alarms::RemoveResult *>(ret2.get());
        REQUIRE(result2 != nullptr);
        REQUIRE(result2->succeed());

        REQUIRE(alarmsRecordInterface.GetCount() == 0);
    }

    SECTION("Edit record with query")
    {
        auto entryQueryGet = std::make_shared<db::query::alarms::Get>(1);
        auto entryRetGet   = alarmsRecordInterface.runQuery(entryQueryGet);
        auto entryResult   = dynamic_cast<db::query::alarms::GetResult *>(entryRetGet.get());
        REQUIRE(entryResult != nullptr);

        auto record   = entryResult->getResult();
        record.time   = TimePointFromString("2021-01-01 09:00:00");
        record.snooze = 1;
        record.status = AlarmStatus::On;
        record.repeat = 0;
        record.path   = "newFile.wav";

        auto query  = std::make_shared<db::query::alarms::Edit>(record);
        auto ret    = alarmsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarms::EditResult *>(ret.get());
        REQUIRE(result != nullptr);

        getQuery(1, TimePointFromString("2021-01-01 09:00:00"), 1, AlarmStatus::On, 0, "newFile.wav");
    }

    SECTION("Add with query")
    {
        AlarmsTableRow tableRow3(4, TimePointFromString("2020-11-11 09:00:00"), 0, AlarmStatus::Off, 9, "music.wav");
        auto record = AlarmsRecord(tableRow3);
        auto query  = std::make_shared<db::query::alarms::Add>(record);
        auto ret    = alarmsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::alarms::AddResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->succeed());
        auto maxID = alarmsRecordInterface.GetCount();

        getQuery(maxID, TimePointFromString("2020-11-11 09:00:00"), 0, AlarmStatus::Off, 9, "music.wav");
    }

    Database::deinitialize();
}
