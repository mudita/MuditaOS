// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common.hpp"

#include <Database/Database.hpp>
#include <Databases/AlarmsDB.hpp>
#include <Tables/AlarmsTable.hpp>

#include <catch2/catch.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>

TEST_CASE("Alarms Table tests")
{
    Database::initialize();

    const auto alarmsPath = (std::filesystem::path{"sys/user"} / "alarms.db");
    RemoveDbFiles(alarmsPath.stem());

    AlarmsDB alarmsDb(alarmsPath.c_str());
    REQUIRE(alarmsDb.isInitialized());

    auto &alarmsTbl = alarmsDb.alarms;
    REQUIRE(alarmsTbl.count() == 0);

    REQUIRE(alarmsTbl.add(
        AlarmsTableRow(1, TimePointFromString("2020-11-11 15:10:00"), 0, AlarmStatus::Off, 0, "file.mp3")));
    REQUIRE(alarmsTbl.add(
        AlarmsTableRow(2, TimePointFromString("2020-11-11 15:15:00"), 1, AlarmStatus::On, 1, "file2.mp3")));

    REQUIRE(alarmsTbl.count() == 2);
    REQUIRE(alarmsTbl.countByFieldId("status", 0) == 1);

    SECTION("Default Constructor")
    {
        AlarmsTableRow test;
        REQUIRE(test.time == TIME_POINT_INVALID);
        REQUIRE(test.snooze == 0);
        REQUIRE(test.status == AlarmStatus::On);
        REQUIRE(test.repeat == 0);
        REQUIRE(test.path == "");
    }

    SECTION("Get entry by ID")
    {
        auto entry = alarmsTbl.getById(1);
        REQUIRE(entry.ID == 1);
        REQUIRE(entry.time == TimePointFromString("2020-11-11 15:10:00"));
        REQUIRE(entry.snooze == 0);
        REQUIRE(entry.status == AlarmStatus::Off);
        REQUIRE(entry.repeat == 0);
        REQUIRE(entry.path == "file.mp3");
        REQUIRE(entry.isValid());
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = alarmsTbl.getById(100);
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.time == TIME_POINT_INVALID);
        REQUIRE(entry.snooze == 0);
        REQUIRE(entry.status == AlarmStatus::On);
        REQUIRE(entry.repeat == 0);
        REQUIRE(entry.path == "");
        REQUIRE_FALSE(entry.isValid());
    }

    SECTION("Entry update")
    {
        auto entryUpdate   = alarmsTbl.getById(1);
        entryUpdate.time   = TimePointFromString("2020-12-31 23:59:00");
        entryUpdate.snooze = 0;
        entryUpdate.status = AlarmStatus::On;
        entryUpdate.path   = "musicFileUpdate.mp3";

        REQUIRE(alarmsTbl.update(entryUpdate));

        auto entry = alarmsTbl.getById(1);
        REQUIRE(entry.ID == entryUpdate.ID);
        REQUIRE(entry.time == entryUpdate.time);
        REQUIRE(entry.snooze == entryUpdate.snooze);
        REQUIRE(entry.status == entryUpdate.status);
        REQUIRE(entry.repeat == entryUpdate.repeat);
        REQUIRE(entry.path == entryUpdate.path);
    }

    SECTION("Remove entries")
    {
        REQUIRE(alarmsTbl.removeById(2));
        REQUIRE(alarmsTbl.count() == 1);

        REQUIRE(alarmsTbl.removeById(100));

        REQUIRE(alarmsTbl.removeById(1));
        REQUIRE(alarmsTbl.count() == 0);
    }

    SECTION("Turn off all alarms")
    {
        REQUIRE(alarmsTbl.updateStatuses(AlarmStatus::Off));
        auto retOffsetLimit = alarmsTbl.getLimitOffset(0, alarmsTbl.count());
        for (const auto &record : retOffsetLimit) {
            REQUIRE(record.status == AlarmStatus::Off);
        }
    }

    SECTION("Get limit/offset")
    {
        auto retOffsetLimit = alarmsTbl.getLimitOffset(0, 2);
        REQUIRE(retOffsetLimit.size() == 2);

        retOffsetLimit = alarmsTbl.getLimitOffset(0, 100);
        REQUIRE(retOffsetLimit.size() == 2);

        retOffsetLimit = alarmsTbl.getLimitOffset(5, 5);
        REQUIRE(retOffsetLimit.empty());

        retOffsetLimit = alarmsTbl.getLimitOffsetByField(
            0, 5, AlarmsTableFields::Status, std::to_string(static_cast<int>(AlarmStatus::Off)).c_str());
        REQUIRE(retOffsetLimit.size() == 1);

        retOffsetLimit = alarmsTbl.getLimitOffsetByField(0, 5, AlarmsTableFields::Path, ".mp3");
        REQUIRE(retOffsetLimit.empty());
    }

    SECTION("Check limit/offset sorting correctness")
    {
        REQUIRE(alarmsTbl.add(
            AlarmsTableRow(3, TimePointFromString("2020-11-12 17:10:00"), 0, AlarmStatus::Off, 0, "file.mp3")));
        REQUIRE(alarmsTbl.add(
            AlarmsTableRow(4, TimePointFromString("2020-11-11 19:25:00"), 1, AlarmStatus::On, 1, "file2.mp3")));
        REQUIRE(alarmsTbl.add(
            AlarmsTableRow(5, TimePointFromString("2020-12-11 07:15:00"), 1, AlarmStatus::On, 1, "file2.mp3")));

        const std::array<TimePoint, 5> paramTime{TimePointFromString("2020-12-11 07:15:00"),
                                                 TimePointFromString("2020-11-11 15:10:00"),
                                                 TimePointFromString("2020-11-11 15:15:00"),
                                                 TimePointFromString("2020-11-12 17:10:00"),
                                                 TimePointFromString("2020-11-11 19:25:00")};

        REQUIRE(alarmsTbl.count() == 5);
        auto entries   = alarmsTbl.getLimitOffset(0, 5);
        uint32_t index = 0;
        for (const auto &entry : entries) {
            REQUIRE(entry.time == paramTime[index]);
            REQUIRE(entry.isValid());
            ++index;
        }
    }

    SECTION("Remove by field")
    {
        REQUIRE(alarmsTbl.updateStatuses(AlarmStatus::Off));
        REQUIRE(alarmsTbl.removeByField(AlarmsTableFields::Status,
                                        std::to_string(static_cast<int>(AlarmStatus::Off)).c_str()));
        REQUIRE(alarmsTbl.count() == 0);
    }

    Database::deinitialize();
}
