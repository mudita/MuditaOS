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
#include "module-db/queries/calendar/QueryEventsEdit.hpp"

#include <vfs.hpp>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>

TEST_CASE("Events Record tests")
{
    Database::initialize();

    vfs.remove(EventsDB::GetDBName());

    EventsDB eventsDb;

    REQUIRE(eventsDb.isInitialized());

    SECTION("Default Constructor")
    {
        EventsRecord testRec;
        REQUIRE(testRec.title == "");
        REQUIRE(testRec.reminder == 0);
        REQUIRE(testRec.repeat == 0);
    }

    SECTION("Constructor from EventsTableRow")
    {
        EventsTableRow tableRow{{.ID = 10},
                                "Event3",
                                TimePointFromString("2019-10-20 14:24"),
                                TimePointFromString("2019-10-20 15:36"),
                                1,
                                2};
        EventsRecord testRec(tableRow);
        REQUIRE(testRec.title == "Event3");
        REQUIRE(testRec.date_from == TimePointFromString("2019-10-20 14:24"));
        REQUIRE(testRec.date_till == TimePointFromString("2019-10-20 15:36"));
        REQUIRE(testRec.reminder == 1);
        REQUIRE(testRec.repeat == 2);
        REQUIRE(testRec.isValid());
    }

    EventsRecord testRec;
    EventsRecordInterface eventsRecordInterface(&eventsDb);

    auto numberOfEvents = eventsRecordInterface.GetCount();
    REQUIRE(numberOfEvents == 0);

    EventsTableRow tableRow{
        {.ID = 10}, "Event1", TimePointFromString("2019-10-20 14:24"), TimePointFromString("2019-10-20 15:36"), 1, 2};
    auto rec = EventsRecord(tableRow);
    REQUIRE(rec.title == "Event1");
    REQUIRE(rec.date_from == TimePointFromString("2019-10-20 14:24"));
    REQUIRE(rec.date_till == TimePointFromString("2019-10-20 15:36"));
    REQUIRE(rec.reminder == 1);
    REQUIRE(rec.repeat == 2);
    REQUIRE(rec.isValid());

    REQUIRE(eventsRecordInterface.Add(rec));

    numberOfEvents = eventsRecordInterface.GetCount();
    REQUIRE(numberOfEvents == 4);

    SECTION("Get entry by ID")
    {
        auto entry = eventsRecordInterface.GetByID(1);
        REQUIRE(entry.ID == 1);
        REQUIRE(entry.title == "Event1");
        REQUIRE(entry.date_from == TimePointFromString("2019-10-20 14:24"));
        REQUIRE(entry.date_till == TimePointFromString("2019-10-20 15:36"));
        REQUIRE(entry.reminder == 1);
        REQUIRE(entry.repeat == 2);
        REQUIRE(entry.isValid());
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

    EventsTableRow tableRow2{
        {.ID = 10}, "Event2", TimePointFromString("2025-10-20 14:24"), TimePointFromString("2025-10-20 15:36"), 1, 2};
    auto rec2 = EventsRecord(tableRow2);
    REQUIRE(rec2.title == "Event2");
    REQUIRE(rec2.date_from == TimePointFromString("2025-10-20 14:24"));
    REQUIRE(rec2.date_till == TimePointFromString("2025-10-20 15:36"));
    REQUIRE(rec2.reminder == 1);
    REQUIRE(rec2.repeat == 2);
    REQUIRE(rec2.isValid());

    REQUIRE(eventsRecordInterface.Add(rec2));

    numberOfEvents = eventsRecordInterface.GetCount();
    REQUIRE(numberOfEvents == 8);

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
            auto retOffsetLimit = eventsRecordInterface.Select(TimePointFromString("2010-10-20 14:24"),
                                                               TimePointFromString("2010-10-20 15:36"));
            REQUIRE(retOffsetLimit->size() == 0);
        }
    }

    SECTION("Entry update value")
    {
        auto entryPre      = eventsRecordInterface.GetByID(1);
        entryPre.title     = "newTitle";
        entryPre.date_from = TimePointFromString("2019-12-31 23:59");
        entryPre.date_till = TimePointFromString("2019-12-31 23:59");
        REQUIRE(eventsRecordInterface.Update(entryPre));

        auto entry = eventsRecordInterface.GetByID(1);
        REQUIRE(entry.ID == entryPre.ID);
        REQUIRE(entry.title == entryPre.title);
        REQUIRE(entry.date_from == entryPre.date_from);
        REQUIRE(entry.date_till == entryPre.date_till);
        REQUIRE(entry.reminder == entryPre.reminder);
        REQUIRE(entry.repeat == entryPre.repeat);
    }

    EventsTableRow tableRow3{
        {.ID = 3}, "Event3", TimePointFromString("2021-10-20 14:24"), TimePointFromString("2021-10-20 15:36"), 1, 2};
    auto rec3 = EventsRecord(tableRow3);
    REQUIRE(rec3.title == "Event3");
    REQUIRE(rec3.date_from == TimePointFromString("2018-10-20 14:24"));
    REQUIRE(rec3.date_till == TimePointFromString("2021-10-20 15:36"));
    REQUIRE(rec3.reminder == 1);
    REQUIRE(rec3.repeat == 2);
    REQUIRE(rec3.isValid());

    REQUIRE(eventsRecordInterface.Add(rec3));

    auto getQuery = [&](uint32_t id, std::string title, TimePoint date_from, TimePoint date_till) {
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
                           TimePoint date_from,
                           TimePoint date_till,
                           TimePoint filter_from,
                           TimePoint filter_till) {
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

    auto AddQuery = [&](uint32_t id, std::string title, TimePoint date_from, TimePoint date_till) {
        EventsTableRow tableRow2{{.ID = id}, title, date_from, date_till, 1, 2};
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
                         TimePoint date_from,
                         TimePoint date_till,
                         uint32_t reminder,
                         uint32_t repeat) {
        EventsTableRow tableRow2{{.ID = id}, title, date_from, date_till, reminder, repeat};
        auto record = EventsRecord(tableRow2);
        auto query  = std::make_shared<db::query::events::Edit>(record);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::EditResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());

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

    SECTION("Get via query")
    {
        getQuery(3, "Event1", TimePointFromString("2021-10-20 14:24"), TimePointFromString("2021-10-20 15:36"));
    }

    SECTION("GetFiltered via query")
    {
        getFiltered(1,
                    "Event1",
                    TimePointFromString("2019-10-20 14:24"),
                    TimePointFromString("2019-10-20 15:36"),
                    TimePointFromString("2018-10-20 14:24"),
                    TimePointFromString("2019-10-20 15:36"));
    }

    SECTION("Add via query")
    {
        AddQuery(3, "Event3", TimePointFromString("2026-10-20 14:24"), TimePointFromString("2022-10-20 15:36"));
        getQuery(4, "Event1", TimePointFromString("2026-10-20 14:24"), TimePointFromString("2022-10-20 15:36"));
    }

    SECTION("Remove via query")
    {
        RemoveQuery(2);
        auto query  = std::make_shared<db::query::events::GetAll>();
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetAllResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records->size() == 11);
    }

    SECTION("Update via query")
    {
        EditQuery(2, "Event3", TimePointFromString("2021-10-20 14:24"), TimePointFromString("2021-10-20 15:36"), 1, 2);
    }

    SECTION("Get All via query")
    {
        auto query  = std::make_shared<db::query::events::GetAll>();
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetAllResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records->size() == 12);
    }

    SECTION("Get All Limited via query")
    {
        AddQuery(3, "Event1", TimePointFromString("2026-10-20 14:24"), TimePointFromString("2022-10-20 15:36"));
        AddQuery(3, "Event2", TimePointFromString("2020-10-20 14:24"), TimePointFromString("2022-10-20 15:36"));
        AddQuery(3, "Event3", TimePointFromString("2025-10-20 14:24"), TimePointFromString("2022-10-20 15:36"));
        AddQuery(3, "Event4", TimePointFromString("2019-10-20 14:24"), TimePointFromString("2022-10-20 15:36"));
        auto count1     = getAllLimited(0, 2);
        auto count2     = getAllLimited(2, 5);
        auto &eventsTbl = eventsDb.events;
        auto count3     = eventsTbl.count();
        REQUIRE(*count1 == *count2);
        REQUIRE(*count2 == count3);
    }

    Database::deinitialize();
}
