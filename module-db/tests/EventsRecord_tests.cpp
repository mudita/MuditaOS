#include <catch2/catch.hpp>

#include "Interface/EventsRecord.hpp"
#include "Database/Database.hpp"
#include "Databases/EventsDB.hpp"
#include "module-db/queries/calendar/QueryEventsGet.hpp"
#include "module-db/queries/calendar/QueryEventsGetAll.hpp"
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
        REQUIRE(testRec.description == "");
        REQUIRE(testRec.date_from == 0);
        REQUIRE(testRec.date_till == 0);
        REQUIRE(testRec.reminder == 0);
        REQUIRE(testRec.repeat == 0);
        REQUIRE(testRec.time_zone == 0);
    }

    SECTION("Constructor from EventsTableRow")
    {
        EventsTableRow tableRow{{.ID = 10}, "Event3", "Desc3", 1910201424, 1910201536, 1, 2, 1};
        EventsRecord testRec(tableRow);
        REQUIRE(testRec.title == "Event3");
        REQUIRE(testRec.description == "Desc3");
        REQUIRE(testRec.date_from == 1910201424);
        REQUIRE(testRec.date_till == 1910201536);
        REQUIRE(testRec.reminder == 1);
        REQUIRE(testRec.repeat == 2);
        REQUIRE(testRec.time_zone == 1);
        REQUIRE(testRec.isValid());
    }

    EventsRecord testRec;
    EventsRecordInterface eventsRecordInterface(&eventsDb);

    auto numberOfEvents = eventsRecordInterface.GetCount();
    REQUIRE(numberOfEvents == 0);

    EventsTableRow tableRow{{.ID = 10}, "Event1", "Desc1", 1910201424, 1910201536, 1, 2, 1};
    auto rec = EventsRecord(tableRow);
    REQUIRE(rec.title == "Event1");
    REQUIRE(rec.description == "Desc1");
    REQUIRE(rec.date_from == 1910201424);
    REQUIRE(rec.date_till == 1910201536);
    REQUIRE(rec.reminder == 1);
    REQUIRE(rec.repeat == 2);
    REQUIRE(rec.time_zone == 1);
    REQUIRE(rec.isValid());

    REQUIRE(eventsRecordInterface.Add(rec));

    numberOfEvents = eventsRecordInterface.GetCount();
    REQUIRE(numberOfEvents == 1);

    SECTION("Get entry by ID")
    {
        auto entry = eventsRecordInterface.GetByID(1);
        REQUIRE(entry.ID == 1);
        REQUIRE(entry.title == "Event1");
        REQUIRE(entry.description == "Desc1");
        REQUIRE(entry.date_from == 1910201424);
        REQUIRE(entry.date_till == 1910201536);
        REQUIRE(entry.reminder == 1);
        REQUIRE(entry.repeat == 2);
        REQUIRE(entry.time_zone == 1);
        REQUIRE(entry.isValid());
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = eventsRecordInterface.GetByID(100);
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.title == "");
        REQUIRE(entry.description == "");
        REQUIRE(entry.date_from == 0);
        REQUIRE(entry.date_till == 0);
        REQUIRE(entry.reminder == 0);
        REQUIRE(entry.repeat == 0);
        REQUIRE(entry.time_zone == 0);
        REQUIRE_FALSE(entry.isValid());
    }

    EventsTableRow tableRow2{{.ID = 10}, "Event2", "Desc2", 2510201424, 2510201536, 1, 2, 1};
    auto rec2 = EventsRecord(tableRow2);
    REQUIRE(rec2.title == "Event2");
    REQUIRE(rec2.description == "Desc2");
    REQUIRE(rec2.date_from == 2510201424);
    REQUIRE(rec2.date_till == 2510201536);
    REQUIRE(rec2.reminder == 1);
    REQUIRE(rec2.repeat == 2);
    REQUIRE(rec2.time_zone == 1);
    REQUIRE(rec2.isValid());

    REQUIRE(eventsRecordInterface.Add(rec2));

    numberOfEvents = eventsRecordInterface.GetCount();
    REQUIRE(numberOfEvents == 2);

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
            REQUIRE(retOffsetLimit->size() == 0);
        }

        SECTION("0 - get all")
        {
            auto retOffsetLimit = eventsRecordInterface.GetLimitOffset(0, 0);
            REQUIRE(retOffsetLimit->size() == numberOfEvents);
        }

        SECTION("Get table rows by SELECT")
        {
            auto retOffsetLimit = eventsRecordInterface.Select(1810201424, 1911201536);
            REQUIRE(retOffsetLimit->size() == 1);
            auto entry = retOffsetLimit->back();
            REQUIRE(entry.ID == 1);
            REQUIRE(entry.title == "Event1");
            REQUIRE(entry.description == "Desc1");
            REQUIRE(entry.date_from == 1910201424);
            REQUIRE(entry.date_till == 1910201536);
            REQUIRE(entry.reminder == 1);
            REQUIRE(entry.repeat == 2);
            REQUIRE(entry.time_zone == 1);
            REQUIRE(entry.isValid());
        }
    }

    SECTION("Entry update value")
    {
        auto entryPre        = eventsRecordInterface.GetByID(1);
        auto checkV          = entryPre.date_from;
        entryPre.title       = "newTitle";
        entryPre.description = "newDesc";
        entryPre.date_from   = 1999999999;
        entryPre.date_till   = 1999999999;
        LOG_DEBUG("LOG ON!!!!!!!!!!!!!!!!!!");
        REQUIRE(eventsRecordInterface.Update(entryPre, checkV));

        auto entry = eventsRecordInterface.GetByID(1);
        REQUIRE(entry.ID == entryPre.ID);
        REQUIRE(entry.title == entryPre.title);
        REQUIRE(entry.description == entryPre.description);
        REQUIRE(entry.date_from == entryPre.date_from);
        REQUIRE(entry.date_till == entryPre.date_till);
        REQUIRE(entry.reminder == entryPre.reminder);
        REQUIRE(entry.repeat == entryPre.repeat);
        REQUIRE(entry.time_zone == entryPre.time_zone);
    }

    EventsTableRow tableRow3{{.ID = 3}, "Event3", "Desc3", 2110201424, 2110201536, 1, 2, 1};
    auto rec3 = EventsRecord(tableRow3);
    REQUIRE(rec3.title == "Event3");
    REQUIRE(rec3.description == "Desc3");
    REQUIRE(rec3.date_from == 2110201424);
    REQUIRE(rec3.date_till == 2110201536);
    REQUIRE(rec3.reminder == 1);
    REQUIRE(rec3.repeat == 2);
    REQUIRE(rec3.time_zone == 1);
    REQUIRE(rec3.isValid());

    REQUIRE(eventsRecordInterface.Add(rec3));

    numberOfEvents = eventsRecordInterface.GetCount();
    REQUIRE(numberOfEvents == 3);

    auto getQuery =
        [&](uint32_t id, std::string title, std::string description, uint32_t date_from, uint32_t date_till) {
            auto query  = std::make_shared<db::query::events::Get>(id);
            auto ret    = eventsRecordInterface.runQuery(query);
            auto result = dynamic_cast<db::query::events::GetResult *>(ret.get());
            REQUIRE(result != nullptr);
            auto record = result->getResult();
            REQUIRE(record.isValid());
            REQUIRE(record.title == title);
            REQUIRE(record.description == description);
            REQUIRE(record.date_from == date_from);
            REQUIRE(record.date_till == date_till);

            return record.date_from;
        };

    auto getFiltered = [&](uint32_t id,
                           std::string title,
                           std::string description,
                           uint32_t date_from,
                           uint32_t date_till,
                           uint32_t filter_from,
                           uint32_t filter_till) {
        auto query  = std::make_shared<db::query::events::GetFiltered>(filter_from, filter_till);
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetFilteredResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        auto record  = records->back();
        REQUIRE(record.isValid());
        REQUIRE(record.title == title);
        REQUIRE(record.description == description);
        REQUIRE(record.date_from == date_from);
        REQUIRE(record.date_till == date_till);
    };

    auto AddQuery =
        [&](uint32_t id, std::string title, std::string description, uint32_t date_from, uint32_t date_till) {
            EventsTableRow tableRow2{{.ID = id}, title, description, date_from, date_till, 1, 2, 1};
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
                         std::string description,
                         uint32_t date_from,
                         uint32_t date_till,
                         uint32_t reminder,
                         uint32_t repeat,
                         uint32_t time_zone,
                         uint32_t checkV) {
        EventsTableRow tableRow2{{.ID = id}, title, description, date_from, date_till, reminder, repeat, time_zone};
        auto record = EventsRecord(tableRow2);
        auto query  = std::make_shared<db::query::events::Edit>(record, checkV);
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
        REQUIRE(recordGet.description == description);
        REQUIRE(recordGet.date_from == date_from);
        REQUIRE(recordGet.date_till == date_till);
        REQUIRE(recordGet.reminder == reminder);
        REQUIRE(recordGet.repeat == repeat);
        REQUIRE(recordGet.time_zone == time_zone);
    };

    SECTION("Get via query")
    {
        getQuery(3, "Event3", "Desc3", 2110201424, 2110201536);
    }

    SECTION("GetFiltered via query")
    {
        getFiltered(1, "Event1", "Desc1", 1910201424, 1910201536, 1810201424, 1912201536);
    }

    SECTION("Add via query")
    {
        AddQuery(3, "Event3", "Desc3", 2610201424, 2210201536);
        getQuery(4, "Event3", "Desc3", 2610201424, 2210201536);
    }

    SECTION("Remove via query")
    {
        RemoveQuery(2);
        auto query  = std::make_shared<db::query::events::GetAll>();
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetAllResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records->size() == 3);
    }

    SECTION("Update via query")
    {
        uint32_t checkV = getQuery(2, "Event2", "Desc2", 2510201424, 2510201536);
        EditQuery(2, "Event3", "Desc2", 2110201424, 2110201536, 1, 2, 2, checkV);
    }

    SECTION("Get All via query")
    {
        auto query  = std::make_shared<db::query::events::GetAll>();
        auto ret    = eventsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::events::GetAllResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records->size() == 3);
    }

    Database::deinitialize();
}
