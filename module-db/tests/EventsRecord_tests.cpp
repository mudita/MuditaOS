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

using namespace std::chrono_literals;

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

    const auto eventsPath = (purefs::dir::getUserDiskPath() / "events.db").c_str();
    std::filesystem::remove(eventsPath);

    EventsDB eventsDb{eventsPath};

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

    EventsRecord testRec;
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
    REQUIRE(numberOfEvents == 4);

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
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.title == "");
        REQUIRE(entry.date_from == TIME_POINT_INVALID);
        REQUIRE(entry.date_till == TIME_POINT_INVALID);
        REQUIRE(entry.reminder == 0);
        REQUIRE(entry.repeat == 0);
        REQUIRE_FALSE(entry.isValid());
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
        }

        SECTION("Get table rows using bigger limit parameters")
        {
            auto retOffsetLimit = eventsRecordInterface.GetLimitOffset(0, 100);
            REQUIRE(retOffsetLimit->size() == numberOfEvents);
        }

        SECTION("Get table rows using invalid offset/limit parameters(should return empty object)")
        {
            auto retOffsetLimit = eventsRecordInterface.GetLimitOffset(5, 4);
            REQUIRE(retOffsetLimit->size() == 3);
        }

        SECTION("0 - get all")
        {
            auto retOffsetLimit = eventsRecordInterface.GetLimitOffset(0, 0);
            REQUIRE(retOffsetLimit->size() == numberOfEvents);
        }

        SECTION("Get table rows by SELECT invalid")
        {
            auto retOffsetLimit = eventsRecordInterface.Select(
                TimePointFromString("2010-10-20 14:24:00"), TimePointFromString("2010-10-20 15:36:00"), 0, UINT32_MAX);
            REQUIRE(retOffsetLimit.size() == 0);
        }
    }

        SECTION("Biweekly, count = 4")
        {
            uint32_t expectedEventsCount = 4;
            auto entries                 = set_repeat(Repeat::biweekly, expectedEventsCount);

        auto entry = eventsRecordInterface.GetByID(1);
        REQUIRE(entry.ID == entryPre.ID);
        REQUIRE(entry.title == entryPre.title);
        REQUIRE(entry.date_from == entryPre.date_from);
        REQUIRE(entry.date_till == entryPre.date_till);
        REQUIRE(entry.reminder == entryPre.reminder);
        REQUIRE(entry.repeat == entryPre.repeat);
    }

    REQUIRE(numberOfEvents == 8);

    SECTION("Select first upcoming event")
    {
        utils::time::TimePoint start_date = TimePointFromString("2019-10-19 14:24:00");
        auto nextUpcoming    = eventsRecordInterface.SelectFirstUpcoming(start_date, start_date);
        REQUIRE(nextUpcoming->size() == 1);
        EventsRecord nextEventsRecord = nextUpcoming->at(0);
        REQUIRE(nextEventsRecord.ID == 7);

        start_date   = TimePointFromString("2019-10-23 14:24:00");
        nextUpcoming = eventsRecordInterface.SelectFirstUpcoming(start_date, start_date);
        REQUIRE(nextUpcoming->size() == 0);
    }

    ///====TEST QUERY====
    auto getQuery = [&](uint32_t id, EventsRecord expected_record) {
        auto query  = std::make_shared<db::query::events::Get>(id);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto record = result->getResult();
        REQUIRE(record.isValid());
        REQUIRE(record.title == title);

        return record.date_from;
    };

    auto getFiltered = [&](uint32_t id,
                           std::string title,
                           utils::time::TimePoint date_from,
                           utils::time::TimePoint date_till,
                           utils::time::TimePoint filter_from,
                           utils::time::TimePoint filter_till) {
        auto query  = std::make_shared<db::query::events::GetFiltered>(filter_from, filter_till);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetFilteredResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records->size() == 0);
    };

    auto getAllLimited = [&](uint32_t offset, uint32_t limit) {
        auto query  = std::make_shared<db::query::events::GetAllLimited>(offset, limit);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetAllLimitedResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records->size() == limit);
        auto count = result->getCountResult();
        return count;
    };

    auto AddQuery = [&](uint32_t id, std::string title, utils::time::TimePoint date_from, utils::time::TimePoint date_till) {
        EventsTableRow tableRow2{{.ID = id}, "test", title, date_from, date_till, 1, 2, TIME_POINT_INVALID};
        auto record = EventsRecord(tableRow2);
        auto query  = std::make_shared<db::query::events::Add>(record);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::AddResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
    };

    auto RemoveQuery = [&](uint32_t id) {
        auto query  = std::make_shared<db::query::events::Remove>(id);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::RemoveResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
    };

    auto EditQuery = [&](uint32_t id,
                         std::string title,
                         utils::time::TimePoint date_from,
                         utils::time::TimePoint date_till,
                         uint32_t reminder,
                         uint32_t repeat) {
        EventsTableRow tableRow2{{.ID = id}, "test", title, date_from, date_till, reminder, repeat, TIME_POINT_INVALID};
        auto record = EventsRecord(tableRow2);
        auto query  = std::make_shared<db::query::events::Edit>(record);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::EditResult *>(ret.get());
        REQUIRE(result != nullptr);

        auto queryGet  = std::make_shared<db::query::events::Get>(id);
        auto retGet    = eventsRecordInterface.runQuery(queryGet);
        auto resultGet = dynamic_cast<db::query::events::GetResult *>(retGet.get());
        REQUIRE(result != nullptr);
        auto recordGet = resultGet->getResult();
        REQUIRE(recordGet.isValid());
        REQUIRE(recordGet.title == title);
        REQUIRE(recordGet.reminder == reminder);
        REQUIRE(recordGet.repeat == repeat);
    };

    auto selectFirstUpcomingEvent = [&](utils::time::TimePoint filter_from, utils::time::TimePoint filter_till) {
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
        REQUIRE(records->size() == 1);
        auto firstEvent = records->at(0);
        REQUIRE(firstEvent.ID == 7);

        date_from = TimePointFromString("2025-11-10 14:24:00");
        date_till = TimePointFromString("2026-10-20 14:24:00");
        records   = selectFirstUpcomingEvent(date_from, date_till);
        REQUIRE(records->size() == 0);
    }

    Database::deinitialize();
}
