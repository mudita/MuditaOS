// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Interface/EventsRecord.hpp"
#include "Database/Database.hpp"
#include "Databases/EventsDB.hpp"
#include "module-db/queries/calendar/QueryEventsGet.hpp"
#include "module-db/queries/calendar/QueryEventsGetAll.hpp"
#include "module-db/queries/calendar/QueryEventsGetAllLimited.hpp"
#include "module-db/queries/calendar/QueryEventsGetFiltered.hpp"
#include "module-db/queries/calendar/QueryEventsAdd.hpp"
#include "module-db/queries/calendar/QueryEventsRemove.hpp"
#include "module-db/queries/calendar/QueryEventsRemoveICS.hpp"
#include "module-db/queries/calendar/QueryEventsEdit.hpp"
#include "module-db/queries/calendar/QueryEventsEditICS.hpp"
#include "module-db/queries/calendar/QueryEventsSelectFirstUpcoming.hpp"

#include <vfs.hpp>
#include <purefs/filesystem_paths.hpp>

#include <stdint.h>
#include <algorithm>
#include <iostream>

static auto remove_events(EventsDB &db) -> bool
{
    auto count   = db.events.count();
    auto records = db.events.getLimitOffset(0, count);
    bool ret     = true;
    for (auto rec : records) {
        ret = ret && db.events.removeById(rec.ID);
    }
    return ret;
}

TEST_CASE("Events Record tests")
{
    Database::initialize();

    const auto eventsPath = purefs::dir::getUserDiskPath() / "events.db";
    std::filesystem::remove(eventsPath);

    EventsDB eventsDb{eventsPath.c_str()};

    REQUIRE(eventsDb.isInitialized());

    SECTION("Default Constructor")
    {
        EventsRecord testRec;
        REQUIRE(testRec.title == "");
        REQUIRE(testRec.reminder == 0);
        REQUIRE(testRec.repeat == 0);
    }

    EventsTableRow testRow = {{1},
                              .UID              = "test1",
                              .title            = "Event1",
                              .date_from        = TimePointFromString("2019-10-20 14:25:00"),
                              .date_till        = TimePointFromString("2019-10-20 15:36:00"),
                              .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                              .repeat           = static_cast<uint32_t>(Repeat::never),
                              .reminder_fired   = TimePointFromString("2019-10-20 14:20:00"),
                              .provider_type    = "PurePhone",
                              .provider_id      = "testID",
                              .provider_iCalUid = "test6"};

    EventsTableRow testRow2 = {{2},
                               .UID              = "test2",
                               .title            = "Event2",
                               .date_from        = TimePointFromString("2019-10-21 14:24:00"),
                               .date_till        = TimePointFromString("2019-10-21 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TIME_POINT_INVALID,
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    EventsTableRow testRow3 = {{3},
                               .UID              = "test3",
                               .title            = "Event3",
                               .date_from        = TimePointFromString("2019-10-22 14:25:00"),
                               .date_till        = TimePointFromString("2019-10-22 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TimePointFromString("2019-10-22 14:20:00"),
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    EventsTableRow testRow4 = {{4},
                               .UID              = "test4",
                               .title            = "Event4",
                               .date_from        = TimePointFromString("2019-10-23 14:25:00"),
                               .date_till        = TimePointFromString("2019-10-23 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TimePointFromString("2019-10-23 14:20:00"),
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    EventsTableRow testRow5 = {{5},
                               .UID              = "test5",
                               .title            = "Event5",
                               .date_from        = TimePointFromString("2019-10-24 14:25:00"),
                               .date_till        = TimePointFromString("2019-10-24 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TIME_POINT_INVALID,
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    EventsTableRow testRow6 = {{6},
                               .UID              = "test6",
                               .title            = "Event6",
                               .date_from        = TimePointFromString("2019-10-24 14:25:00"),
                               .date_till        = TimePointFromString("2019-10-24 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TimePointFromString("2019-10-24 14:20:00"),
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    auto check_record = [](const EventsRecord &actual, const EventsRecord &expected) {
        CHECK(actual.ID == expected.ID);
        CHECK(actual.UID == expected.UID);
        CHECK(actual.title == expected.title);
        CHECK(TimePointToString(actual.date_from) == TimePointToString(expected.date_from));
        CHECK(TimePointToString(actual.date_till) == TimePointToString(expected.date_till));
        CHECK(actual.reminder == expected.reminder);
        CHECK(actual.repeat == expected.repeat);
        CHECK(actual.reminder_fired == expected.reminder_fired);
        CHECK(actual.provider_type == expected.provider_type);
        CHECK(actual.provider_id == expected.provider_id);
        CHECK(actual.provider_iCalUid == expected.provider_iCalUid);
        CHECK(actual.isValid());
    };

    SECTION("Constructor from EventsTableRow")
    {
        EventsRecord testRecord(testRow);
        check_record(testRecord, testRow);
    }

    EventsRecordInterface eventsRecordInterface(&eventsDb);
    auto numberOfEvents = eventsRecordInterface.GetCount();
    REQUIRE(numberOfEvents == 0);

    auto testRecord  = EventsRecord(testRow);
    auto testRecord2 = EventsRecord(testRow2);
    auto testRecord3 = EventsRecord(testRow3);
    auto testRecord4 = EventsRecord(testRow4);
    auto testRecord5 = EventsRecord(testRow5);
    auto testRecord6 = EventsRecord(testRow6);

    REQUIRE(eventsRecordInterface.Add(testRecord));
    REQUIRE(eventsRecordInterface.Add(testRecord2));
    REQUIRE(eventsRecordInterface.Add(testRecord3));
    REQUIRE(eventsRecordInterface.Add(testRecord4));
    REQUIRE(eventsRecordInterface.Add(testRecord5));
    REQUIRE(eventsRecordInterface.Add(testRecord6));

    numberOfEvents = eventsRecordInterface.GetCount();
    REQUIRE(numberOfEvents == 6);

    SECTION("Get entry by ID")
    {
        auto entry = eventsRecordInterface.GetByID(testRecord3.ID);
        check_record(entry, testRecord3);
    }

    SECTION("Get count filtered")
    {
        auto count = eventsRecordInterface.GetCountFiltered(testRecord3.date_from, testRecord5.date_from);
        CHECK(count == 2);
    }

    SECTION("Get count")
    {
        auto count = eventsRecordInterface.GetCount();
        CHECK(count == 6);
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = eventsRecordInterface.GetByID(100);
        CHECK(entry.ID == DB_ID_NONE);
        CHECK(entry.title == "");
        CHECK(entry.date_from == TIME_POINT_INVALID);
        CHECK(entry.date_till == TIME_POINT_INVALID);
        CHECK(entry.reminder == 0);
        CHECK(entry.repeat == 0);
        CHECK(entry.reminder_fired == TIME_POINT_INVALID);
        CHECK(entry.provider_type == "");
        CHECK(entry.provider_id == "");
        CHECK(entry.provider_iCalUid == "");
        CHECK_FALSE(entry.isValid());
    }

    SECTION("Entry remove")
    {
        REQUIRE(eventsRecordInterface.RemoveByID(testRecord4.ID));
        numberOfEvents = eventsRecordInterface.GetCount();
        REQUIRE(numberOfEvents == 5);
        CHECK_NOTHROW(eventsRecordInterface.GetByID(testRecord4.ID));
    }

    SECTION("Entry remove by uid")
    {
        REQUIRE(eventsRecordInterface.RemoveByUID(testRecord4.UID));
        numberOfEvents = eventsRecordInterface.GetCount();
        REQUIRE(numberOfEvents == 5);
        CHECK_NOTHROW(eventsRecordInterface.GetByID(testRecord4.ID));
    }

    SECTION("Entry update")
    {
        auto entryToUpdate      = eventsRecordInterface.GetByID(testRecord5.ID);
        entryToUpdate.title     = "newTitle";
        entryToUpdate.date_from = TimePointFromString("2019-12-31 23:59:00");
        entryToUpdate.date_till = TimePointFromString("2019-12-31 23:59:00");
        REQUIRE(eventsRecordInterface.Update(entryToUpdate));

        auto entry = eventsRecordInterface.GetByID(entryToUpdate.ID);
        check_record(entry, entryToUpdate);
    }

    SECTION("Entry update by uid")
    {
        auto entryToUpdate      = eventsRecordInterface.GetByID(testRecord3.ID);
        entryToUpdate.title     = "newTitle";
        entryToUpdate.date_from = TimePointFromString("2019-12-31 23:59:00");
        entryToUpdate.date_till = TimePointFromString("2019-12-31 23:59:00");
        REQUIRE(eventsRecordInterface.UpdateByUID(entryToUpdate));

        auto entry = eventsRecordInterface.GetByID(entryToUpdate.ID);
        check_record(entry, entryToUpdate);
    }

    SECTION("Get entries")
    {

        SECTION("Get records using valid offset/limit parameters")
        {
            auto retOffsetLimit = eventsRecordInterface.GetLimitOffset(0, numberOfEvents);
            REQUIRE(retOffsetLimit->size() == numberOfEvents);
            CHECK((*retOffsetLimit)[0].ID == testRecord.ID);
            CHECK((*retOffsetLimit)[1].ID == testRecord2.ID);
            CHECK((*retOffsetLimit)[2].ID == testRecord3.ID);
            CHECK((*retOffsetLimit)[3].ID == testRecord4.ID);
            CHECK((*retOffsetLimit)[4].ID == testRecord5.ID);
            CHECK((*retOffsetLimit)[5].ID == testRecord6.ID);
        }

        SECTION("Get table rows using bigger limit parameters")
        {
            auto retOffsetLimit = eventsRecordInterface.GetLimitOffset(0, 100);
            REQUIRE(retOffsetLimit->size() == numberOfEvents);
        }

        SECTION("Get table rows using invalid offset/limit parameters(should return empty object)")
        {
            auto retOffsetLimit = eventsRecordInterface.GetLimitOffset(6, 10);
            REQUIRE(retOffsetLimit->size() == 0);
        }

        SECTION("0 - get all")
        {
            auto retOffsetLimit = eventsRecordInterface.GetLimitOffset(0, 0);
            REQUIRE(retOffsetLimit->size() == numberOfEvents);
        }

        SECTION("Get table rows by SELECT")
        {
            auto retOffsetLimit = eventsRecordInterface.Select(
                TimePointFromString("2019-10-19 14:24:00"), TimePointFromString("2019-10-27 15:36:00"), 0, UINT32_MAX);
            REQUIRE(retOffsetLimit.size() == numberOfEvents);
        }

        SECTION("Get table rows by SELECT limited")
        {
            auto retOffsetLimit = eventsRecordInterface.Select(
                TimePointFromString("2019-10-19 14:24:00"), TimePointFromString("2019-10-27 15:36:00"), 0, 3);
            REQUIRE(retOffsetLimit.size() == 3);
        }

        SECTION("Get table rows by SELECT limited by date")
        {
            auto retOffsetLimit = eventsRecordInterface.Select(
                TimePointFromString("2019-10-19 14:24:00"), TimePointFromString("2019-10-22 15:36:00"), 0, UINT32_MAX);
            REQUIRE(retOffsetLimit.size() == 2);
        }

        SECTION("Get table rows by SELECT invalid")
        {
            auto retOffsetLimit = eventsRecordInterface.Select(
                TimePointFromString("2010-10-20 14:24:00"), TimePointFromString("2010-10-20 15:36:00"), 0, UINT32_MAX);
            REQUIRE(retOffsetLimit.size() == 0);
        }

        SECTION("Get records using valid offset/limit date parameters")
        {
            auto recordWithEarliestDate      = eventsRecordInterface.GetByID(testRecord3.ID);
            recordWithEarliestDate.date_from = TimePointFromString("1990-10-23 14:25:00");
            REQUIRE(eventsRecordInterface.Update(recordWithEarliestDate));
            auto retOffsetLimit = eventsRecordInterface.GetLimitOffsetByDate(0, numberOfEvents);
            REQUIRE(retOffsetLimit.size() == numberOfEvents);
            CHECK((retOffsetLimit)[0].ID == testRecord3.ID);
            CHECK((retOffsetLimit)[1].ID == testRecord.ID);
            CHECK((retOffsetLimit)[2].ID == testRecord2.ID);
            CHECK((retOffsetLimit)[3].ID == testRecord4.ID);
            CHECK((retOffsetLimit)[4].ID == testRecord5.ID);
            CHECK((retOffsetLimit)[5].ID == testRecord6.ID);
        }

        SECTION("Get records using valid offset/limit date parameters")
        {
            auto recordWithEarliestDate      = eventsRecordInterface.GetByID(testRecord3.ID);
            recordWithEarliestDate.date_from = TimePointFromString("1990-10-23 14:25:00");
            recordWithEarliestDate.date_till = TimePointFromString("1990-10-23 14:25:00");
            REQUIRE(eventsRecordInterface.Update(recordWithEarliestDate));

            auto retOffsetLimit = eventsRecordInterface.GetLimitOffsetByDate(0, numberOfEvents);
            REQUIRE(retOffsetLimit.size() == numberOfEvents);
            CHECK((retOffsetLimit)[0].ID == recordWithEarliestDate.ID);
            CHECK((retOffsetLimit)[1].ID == testRecord.ID);
            CHECK((retOffsetLimit)[2].ID == testRecord2.ID);
            CHECK((retOffsetLimit)[3].ID == testRecord4.ID);
            CHECK((retOffsetLimit)[4].ID == testRecord5.ID);
            CHECK((retOffsetLimit)[5].ID == testRecord6.ID);
        }
    }

    SECTION("Entry add repeated")
    {
        if (eventsRecordInterface.GetCount() > 0) {
            REQUIRE(remove_events(eventsDb));
        }
        REQUIRE(eventsRecordInterface.GetCount() == 0);

        uint32_t testRecordActualId = numberOfEvents + 1;
        testRecord3.ID              = testRecordActualId;
        auto expectedRecordData     = testRecord3;

        auto set_repeat = [&](Repeat option, uint32_t count) {
            expectedRecordData.repeat = static_cast<uint32_t>(option);
            REQUIRE(eventsRecordInterface.Add(expectedRecordData));
            auto entries = eventsRecordInterface.GetLimitOffset(0, 0);
            REQUIRE(entries->size() == count);
            return *entries;
        };

        SECTION("Daily, count = 7")
        {
            uint32_t expectedEventsCount = 7;
            auto entries                 = set_repeat(Repeat::daily, expectedEventsCount);

            for (auto entry : entries) {
                check_record(entry, expectedRecordData);
                expectedRecordData.date_from = expectedRecordData.date_from + 24h;
                expectedRecordData.date_till = expectedRecordData.date_till + 24h;
            }
        }

        SECTION("Weekly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            auto entries                 = set_repeat(Repeat::weekly, expectedEventsCount);

            for (auto entry : entries) {
                check_record(entry, expectedRecordData);
                expectedRecordData.date_from = expectedRecordData.date_from + (7 * 24h);
                expectedRecordData.date_till = expectedRecordData.date_till + (7 * 24h);
            }
        }

        SECTION("Biweekly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            auto entries                 = set_repeat(Repeat::biweekly, expectedEventsCount);

            for (auto entry : entries) {
                check_record(entry, expectedRecordData);
                expectedRecordData.date_from = expectedRecordData.date_from + (2 * 7 * 24h);
                expectedRecordData.date_till = expectedRecordData.date_till + (2 * 7 * 24h);
            }
        }

        SECTION("Monthly, count = 12")
        {
            uint32_t expectedEventsCount = 12;
            expectedRecordData.date_from = TimePointFromString("2019-01-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-01-05 17:00:00");
            auto entries                 = set_repeat(Repeat::monthly, expectedEventsCount);
            check_record(entries[0], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-02-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-02-05 17:00:00");
            check_record(entries[1], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-03-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-03-05 17:00:00");
            check_record(entries[2], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-04-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-04-05 17:00:00");
            check_record(entries[3], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-05-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-05-05 17:00:00");
            check_record(entries[4], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-06-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-06-05 17:00:00");
            check_record(entries[5], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-07-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-07-05 17:00:00");
            check_record(entries[6], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-08-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-08-05 17:00:00");
            check_record(entries[7], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-09-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-09-05 17:00:00");
            check_record(entries[8], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-10-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-10-05 17:00:00");
            check_record(entries[9], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-11-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-11-05 17:00:00");
            check_record(entries[10], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-12-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-12-05 17:00:00");
            check_record(entries[11], expectedRecordData);
        }

        SECTION("Yearly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            expectedRecordData.date_from = TimePointFromString("2019-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-02-28 17:00:00");
            auto entries                 = set_repeat(Repeat::yearly, expectedEventsCount);
            check_record(entries[0], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2020-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2020-02-28 17:00:00");
            check_record(entries[1], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2020-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2020-02-28 17:00:00");
            check_record(entries[2], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2020-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2020-02-28 17:00:00");
            check_record(entries[3], expectedRecordData);
        }
    }

    SECTION("Entry update repeated")
    {
        if (eventsRecordInterface.GetCount() > 0) {
            REQUIRE(remove_events(eventsDb));
        }
        REQUIRE(eventsRecordInterface.GetCount() == 0);
        uint32_t testRecordActualId = numberOfEvents + 1;
        testRecord.ID               = testRecordActualId;
        REQUIRE(eventsRecordInterface.Add(testRecord));

        auto expectedRecordData = eventsRecordInterface.GetByID(testRecord.ID);

        auto set_repeat = [&](Repeat option, uint32_t count) {
            expectedRecordData.repeat = static_cast<uint32_t>(option);
            REQUIRE(eventsRecordInterface.Update(expectedRecordData));
            auto entries = eventsRecordInterface.GetLimitOffset(0, 0);
            REQUIRE(entries->size() == count);
            return *entries;
        };

        SECTION("Daily, count = 7")
        {
            uint32_t expectedEventsCount = 7;
            auto entries                 = set_repeat(Repeat::daily, expectedEventsCount);

            for (auto entry : entries) {
                check_record(entry, expectedRecordData);
                expectedRecordData.date_from = expectedRecordData.date_from + 24h;
                expectedRecordData.date_till = expectedRecordData.date_till + 24h;
            }
        }

        SECTION("Weekly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            auto entries                 = set_repeat(Repeat::weekly, expectedEventsCount);

            for (auto entry : entries) {
                check_record(entry, expectedRecordData);
                expectedRecordData.date_from = expectedRecordData.date_from + (7 * 24h);
                expectedRecordData.date_till = expectedRecordData.date_till + (7 * 24h);
            }
        }

        SECTION("Biweekly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            auto entries                 = set_repeat(Repeat::biweekly, expectedEventsCount);

            for (auto entry : entries) {
                check_record(entry, expectedRecordData);
                expectedRecordData.date_from = expectedRecordData.date_from + (2 * 7 * 24h);
                expectedRecordData.date_till = expectedRecordData.date_till + (2 * 7 * 24h);
            }
        }

        SECTION("Monthly, count = 12")
        {
            uint32_t expectedEventsCount = 12;
            expectedRecordData.date_from = TimePointFromString("2019-01-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-01-05 17:00:00");
            auto entries                 = set_repeat(Repeat::monthly, expectedEventsCount);
            check_record(entries[0], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-02-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-02-05 17:00:00");
            check_record(entries[1], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-03-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-03-05 17:00:00");
            check_record(entries[2], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-04-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-04-05 17:00:00");
            check_record(entries[3], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-05-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-05-05 17:00:00");
            check_record(entries[4], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-06-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-06-05 17:00:00");
            check_record(entries[5], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-07-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-07-05 17:00:00");
            check_record(entries[6], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-08-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-08-05 17:00:00");
            check_record(entries[7], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-09-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-09-05 17:00:00");
            check_record(entries[8], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-10-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-10-05 17:00:00");
            check_record(entries[9], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-11-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-11-05 17:00:00");
            check_record(entries[10], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-12-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-12-05 17:00:00");
            check_record(entries[11], expectedRecordData);
        }

        SECTION("Yearly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            expectedRecordData.date_from = TimePointFromString("2019-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-02-28 17:00:00");
            auto entries                 = set_repeat(Repeat::yearly, expectedEventsCount);
            check_record(entries[0], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2020-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2020-02-28 17:00:00");
            check_record(entries[1], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2021-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2021-02-28 17:00:00");
            check_record(entries[2], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2022-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2022-02-28 17:00:00");
            check_record(entries[3], expectedRecordData);
        }
    }

    SECTION("Entry updateICS repeated")
    {
        if (eventsRecordInterface.GetCount() > 0) {
            REQUIRE(remove_events(eventsDb));
        }
        REQUIRE(eventsRecordInterface.GetCount() == 0);

        REQUIRE(eventsRecordInterface.Add(testRecord));
        auto expectedRecordData = eventsRecordInterface.GetByUID(testRecord.UID);

        auto set_repeat = [&](Repeat option, uint32_t count) {
            expectedRecordData.repeat = static_cast<uint32_t>(option);
            REQUIRE(eventsRecordInterface.UpdateByUID(expectedRecordData));
            auto entries = eventsRecordInterface.GetLimitOffset(0, 0);
            REQUIRE(entries->size() == count);
            return *entries;
        };

        SECTION("Daily, count = 7")
        {
            uint32_t expectedEventsCount = 7;
            auto entries                 = set_repeat(Repeat::daily, expectedEventsCount);

            for (auto entry : entries) {
                check_record(entry, expectedRecordData);
                expectedRecordData.date_from = expectedRecordData.date_from + 24h;
                expectedRecordData.date_till = expectedRecordData.date_till + 24h;
            }
        }

        SECTION("Weekly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            auto entries                 = set_repeat(Repeat::weekly, expectedEventsCount);

            for (auto entry : entries) {
                check_record(entry, expectedRecordData);
                expectedRecordData.date_from = expectedRecordData.date_from + (7 * 24h);
                expectedRecordData.date_till = expectedRecordData.date_till + (7 * 24h);
            }
        }

        SECTION("Biweekly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            auto entries                 = set_repeat(Repeat::biweekly, expectedEventsCount);

            for (auto entry : entries) {
                check_record(entry, expectedRecordData);
                expectedRecordData.date_from = expectedRecordData.date_from + (2 * 7 * 24h);
                expectedRecordData.date_till = expectedRecordData.date_till + (2 * 7 * 24h);
            }
        }

        SECTION("Monthly, count = 12")
        {
            uint32_t expectedEventsCount = 12;
            expectedRecordData.date_from = TimePointFromString("2019-01-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-01-05 17:00:00");
            auto entries                 = set_repeat(Repeat::monthly, expectedEventsCount);
            check_record(entries[0], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-02-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-02-05 17:00:00");
            check_record(entries[1], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-03-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-03-05 17:00:00");
            check_record(entries[2], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-04-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-04-05 17:00:00");
            check_record(entries[3], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-05-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-05-05 17:00:00");
            check_record(entries[4], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-06-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-06-05 17:00:00");
            check_record(entries[5], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-07-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-07-05 17:00:00");
            check_record(entries[6], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-08-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-08-05 17:00:00");
            check_record(entries[7], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-09-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-09-05 17:00:00");
            check_record(entries[8], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-10-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-10-05 17:00:00");
            check_record(entries[9], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-11-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-11-05 17:00:00");
            check_record(entries[10], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2019-12-05 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-12-05 17:00:00");
            check_record(entries[11], expectedRecordData);
        }

        SECTION("Yearly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            expectedRecordData.date_from = TimePointFromString("2019-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2019-02-28 17:00:00");
            auto entries                 = set_repeat(Repeat::yearly, expectedEventsCount);
            check_record(entries[0], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2020-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2020-02-28 17:00:00");
            check_record(entries[1], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2021-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2021-02-28 17:00:00");
            check_record(entries[2], expectedRecordData);

            expectedRecordData.date_from = TimePointFromString("2022-02-28 15:00:00");
            expectedRecordData.date_till = TimePointFromString("2022-02-28 17:00:00");
            check_record(entries[3], expectedRecordData);
        }
    }

    SECTION("Select first upcoming event")
    {
        TimePoint start_date = TimePointFromString("2019-10-19 14:24:00");
        auto nextUpcoming    = eventsRecordInterface.SelectFirstUpcoming(start_date, start_date);
        REQUIRE(nextUpcoming.size() == 1);
        EventsRecord nextEventsRecord = nextUpcoming.at(0);
        REQUIRE(nextEventsRecord.ID == 2);
        nextEventsRecord.reminder_fired = TimePointFromString("2018-12-31 23:59:00");
        REQUIRE(eventsRecordInterface.Update(nextEventsRecord));

        start_date   = TimePointFromString("2019-10-23 14:24:00");
        nextUpcoming = eventsRecordInterface.SelectFirstUpcoming(start_date, start_date);
        REQUIRE(nextUpcoming.size() == 1);
        REQUIRE(nextEventsRecord.ID == 2);
    }

    ///====TEST QUERY====
    auto getQuery = [&](uint32_t id, EventsRecord expected_record) {
        auto query  = std::make_shared<db::query::events::Get>(id);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto record = result->getResult();
        check_record(record, expected_record);
    };

    SECTION("Check Get via query")
    {
        getQuery(testRecord.ID, testRecord);
        getQuery(testRecord3.ID, testRecord3);
        getQuery(testRecord6.ID, testRecord6);
    }

    [[maybe_unused]] auto getAll = [&](std::vector<EventsRecord> expected_records, uint32_t expectedAllRecsNumber) {
        auto query  = std::make_shared<db::query::events::GetAll>();
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetAllResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records.size() == expectedAllRecsNumber);

        auto expected_record = expected_records.begin();
        std::for_each(records.begin(), records.end(), [&expected_record](const EventsRecord &record) {
            CHECK(record.ID == expected_record->ID);
            CHECK(record.UID == expected_record->UID);
            CHECK(record.title == expected_record->title);
            CHECK(TimePointToString(record.date_from) == TimePointToString(expected_record->date_from));
            CHECK(TimePointToString(record.date_till) == TimePointToString(expected_record->date_till));
            CHECK(record.reminder == expected_record->reminder);
            CHECK(record.repeat == expected_record->repeat);
            CHECK(record.reminder_fired == expected_record->reminder_fired);
            CHECK(record.provider_type == expected_record->provider_type);
            CHECK(record.provider_id == expected_record->provider_id);
            CHECK(record.provider_iCalUid == expected_record->provider_iCalUid);
            CHECK(record.isValid());
            expected_record++;
        });
    };

    SECTION("Get All via query")
    {
        std::vector<EventsRecord> expectedRecords;
        expectedRecords.push_back(testRecord);
        expectedRecords.push_back(testRecord2);
        expectedRecords.push_back(testRecord3);
        expectedRecords.push_back(testRecord4);
        expectedRecords.push_back(testRecord5);
        expectedRecords.push_back(testRecord6);
        getAll(expectedRecords, numberOfEvents);
    }

    auto getFiltered = [&](TimePoint filter_from, TimePoint filter_till, std::vector<EventsRecord> expected_records) {
        auto query  = std::make_shared<db::query::events::GetFiltered>(filter_from, filter_till);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetFilteredResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records.size() == expected_records.size());
        auto count = result->getCountResult();
        REQUIRE(count == expected_records.size());

        auto expected_record = expected_records.begin();
        std::for_each(records.begin(), records.end(), [&expected_record](const EventsRecord &record) {
            CHECK(record.ID == expected_record->ID);
            CHECK(record.UID == expected_record->UID);
            CHECK(record.title == expected_record->title);
            CHECK(TimePointToString(record.date_from) == TimePointToString(expected_record->date_from));
            CHECK(TimePointToString(record.date_till) == TimePointToString(expected_record->date_till));
            CHECK(record.reminder == expected_record->reminder);
            CHECK(record.repeat == expected_record->repeat);
            CHECK(record.reminder_fired == expected_record->reminder_fired);
            CHECK(record.provider_type == expected_record->provider_type);
            CHECK(record.provider_id == expected_record->provider_id);
            CHECK(record.provider_iCalUid == expected_record->provider_iCalUid);
            CHECK(record.isValid());
            expected_record++;
        });
    };

    SECTION("GetFiltered via query")
    {
        auto startDate = testRecord2.date_from;
        auto endDate   = testRecord5.date_till;
        std::vector<EventsRecord> expectedFilteredRecords;
        expectedFilteredRecords.push_back(testRecord2);
        expectedFilteredRecords.push_back(testRecord3);
        expectedFilteredRecords.push_back(testRecord4);
        getFiltered(startDate, endDate, expectedFilteredRecords);
    }

    auto getAllLimited = [&](uint32_t offset,
                             uint32_t limit,
                             std::vector<EventsRecord> expected_records,
                             uint32_t expectedAllRecsNumber) {
        auto query  = std::make_shared<db::query::events::GetAllLimited>(offset, limit);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetAllLimitedResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records.size() == limit);
        auto count = result->getCountResult();
        CHECK(count == expectedAllRecsNumber);

        auto expected_record = expected_records.begin();
        std::for_each(records.begin(), records.end(), [&expected_record](const EventsRecord &record) {
            CHECK(record.ID == expected_record->ID);
            CHECK(record.UID == expected_record->UID);
            CHECK(record.title == expected_record->title);
            CHECK(TimePointToString(record.date_from) == TimePointToString(expected_record->date_from));
            CHECK(TimePointToString(record.date_till) == TimePointToString(expected_record->date_till));
            CHECK(record.reminder == expected_record->reminder);
            CHECK(record.repeat == expected_record->repeat);
            CHECK(record.reminder_fired == expected_record->reminder_fired);
            CHECK(record.provider_type == expected_record->provider_type);
            CHECK(record.provider_id == expected_record->provider_id);
            CHECK(record.provider_iCalUid == expected_record->provider_iCalUid);
            CHECK(record.isValid());
            expected_record++;
        });
    };

    SECTION("Get All Limited via query")
    {
        std::vector<EventsRecord> expectedRecords;
        expectedRecords.push_back(testRecord2);
        expectedRecords.push_back(testRecord3);
        expectedRecords.push_back(testRecord4);
        expectedRecords.push_back(testRecord5);
        uint32_t limit = 4;
        getAllLimited(testRecord2.ID - 1, limit, expectedRecords, numberOfEvents);
    }

    auto AddQuery = [&](EventsRecord record) {
        auto query  = std::make_shared<db::query::events::Add>(record);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::AddResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());

        auto entry = eventsRecordInterface.GetByID(record.ID);
        check_record(entry, record);
    };

    SECTION("Add via query")
    {
        EventsTableRow testRow7 = {{7},
                                   .UID              = "test7",
                                   .title            = "Event7",
                                   .date_from        = TimePointFromString("2019-11-24 14:25:00"),
                                   .date_till        = TimePointFromString("2019-12-24 15:36:00"),
                                   .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                                   .repeat           = static_cast<uint32_t>(Repeat::never),
                                   .reminder_fired   = TimePointFromString("2019-10-24 14:20:00"),
                                   .provider_type    = "PurePhone",
                                   .provider_id      = "testID",
                                   .provider_iCalUid = "test6"};

        auto testRecord7 = EventsRecord(testRow7);
        AddQuery(testRecord7);
    }

    auto RemoveQuery = [&](uint32_t id) {
        auto query  = std::make_shared<db::query::events::Remove>(id);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::RemoveResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
        REQUIRE_NOTHROW(eventsRecordInterface.GetByID(id));
    };

    SECTION("Remove via query")
    {
        RemoveQuery(testRecord2.ID);
        REQUIRE_NOTHROW(eventsRecordInterface.GetByID(testRecord2.ID));
        std::vector<EventsRecord> expectedRecords;
        expectedRecords.push_back(testRecord);
        expectedRecords.push_back(testRecord3);
        expectedRecords.push_back(testRecord4);
        expectedRecords.push_back(testRecord5);
        expectedRecords.push_back(testRecord6);
        getAll(expectedRecords, numberOfEvents - 1);
    }

    auto RemoveQueryICS = [&](std::string uid) {
        auto query  = std::make_shared<db::query::events::RemoveICS>(uid);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::RemoveICSResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
        REQUIRE_NOTHROW(eventsRecordInterface.GetByUID(uid));
    };

    SECTION("RemoveICS via query")
    {
        RemoveQueryICS(testRecord2.UID);
        REQUIRE_NOTHROW(eventsRecordInterface.GetByUID(testRecord2.UID));
        std::vector<EventsRecord> expectedRecords;
        expectedRecords.push_back(testRecord);
        expectedRecords.push_back(testRecord3);
        expectedRecords.push_back(testRecord4);
        expectedRecords.push_back(testRecord5);
        expectedRecords.push_back(testRecord6);
        getAll(expectedRecords, numberOfEvents - 1);
    }

    auto EditQuery = [&](uint32_t id, EventsRecord record) {
        auto entryToUpdate             = eventsRecordInterface.GetByID(id);
        entryToUpdate.title            = record.title;
        entryToUpdate.date_from        = record.date_from;
        entryToUpdate.date_till        = record.date_till;
        entryToUpdate.reminder         = record.reminder;
        entryToUpdate.repeat           = record.repeat;
        entryToUpdate.provider_type    = record.provider_type;
        entryToUpdate.provider_id      = record.provider_id;
        entryToUpdate.provider_iCalUid = record.provider_iCalUid;

        auto query  = std::make_shared<db::query::events::Edit>(entryToUpdate);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::EditResult *>(ret.get());
        REQUIRE(result != nullptr);

        auto entry = eventsRecordInterface.GetByID(entryToUpdate.ID);
        record.ID  = entry.ID;
        record.UID = entry.UID;
        check_record(entry, record);
    };

    SECTION("Update via query")
    {
        EditQuery(testRecord.ID, testRecord);
        EditQuery(testRecord3.ID, testRecord3);
        EditQuery(testRecord6.ID, testRecord6);
    }

    auto EditQueryICS = [&](std::string uid, EventsRecord recordUpdateData) {
        auto entryToUpdate             = eventsRecordInterface.GetByUID(uid);
        entryToUpdate.title            = recordUpdateData.title;
        entryToUpdate.date_from        = recordUpdateData.date_from;
        entryToUpdate.date_till        = recordUpdateData.date_till;
        entryToUpdate.reminder         = recordUpdateData.reminder;
        entryToUpdate.repeat           = recordUpdateData.repeat;
        entryToUpdate.provider_type    = recordUpdateData.provider_type;
        entryToUpdate.provider_id      = recordUpdateData.provider_id;
        entryToUpdate.provider_iCalUid = recordUpdateData.provider_iCalUid;

        auto query  = std::make_shared<db::query::events::EditICS>(entryToUpdate);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::EditICSResult *>(ret.get());
        REQUIRE(result != nullptr);

        auto entry           = eventsRecordInterface.GetByID(entryToUpdate.ID);
        recordUpdateData.ID  = entry.ID;
        recordUpdateData.UID = entry.UID;
        check_record(entry, recordUpdateData);
    };

    SECTION("Update ICS via query")
    {
        EditQueryICS(testRecord.UID, testRecord);
        EditQueryICS(testRecord3.UID, testRecord3);
        EditQueryICS(testRecord6.UID, testRecord6);
    }

    [[maybe_unused]] auto selectFirstUpcomingEvent = [&](TimePoint filter_from, TimePoint filter_till) {
        auto query  = std::make_shared<db::query::events::SelectFirstUpcoming>(filter_from, filter_till);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::SelectFirstUpcomingResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        return records;
    };

    SECTION("Select first upcoming via query")
    {
        auto date_from = TimePointFromString("2019-10-19 14:24:00");
        auto date_till = TimePointFromString("2026-10-20 14:24:00");
        auto records   = selectFirstUpcomingEvent(date_from, date_till);
        REQUIRE(records.size() == 1);
        auto firstEvent = records.at(0);
        REQUIRE(firstEvent.ID == 2);

        date_from = TimePointFromString("2025-11-10 14:24:00");
        date_till = TimePointFromString("2026-10-20 14:24:00");
        records   = selectFirstUpcomingEvent(date_from, date_till);
        REQUIRE(records.size() == 0);
    }

    Database::deinitialize();
}
