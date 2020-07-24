//
// Created by tomasz on 23.07.2020.
//

//#include "vfs.hpp"
//
//#include <catch2/catch.hpp>
//
//#include "Database/Database.hpp"
//#include "Databases/EventsDB.hpp"
//
//#include "Tables/SettingsTable.hpp"
//
//#include <algorithm>
//
//#include <cstdint>
//#include <cstdio>
//#include <cstring>
//
// TEST_CASE("Events Table tests")
//{
//    Database::initialize();
//
//    vfs.remove(EventsDB::GetDBName());
//
//    EventsDB eventsdb;
//    REQUIRE(eventsdb.isInitialized());
//
//    EventsTableRow testRow1 = {
//        {.ID = 0},
//        .title "tytul",
//        .description = "opis",
//        .date_from   = 2007242323,
//        .date_till   = 2007262323,
//        .reminder    = 0,
//        .repeat      = 0,
//        .time_zone   = 0,
//
//    };
//
//    // add 5 elements into table
//    REQUIRE(eventsdb.events.add(testRow1));
//    REQUIRE(eventsdb.events.add(testRow1));
//    REQUIRE(eventsdb.events.add(testRow1));
//    REQUIRE(eventsdb.events.add(testRow1));
//    REQUIRE(eventsdb.events.add(testRow1));
//
//    // Table should have 5 elements
//    REQUIRE(eventsdb.events.count() == 5);
//
//    // update existing element in table
//    testRow1.ID    = 5;
//    testRow1.title = "updated Test Events message ";
//    REQUIRE(eventsdb.events.update(testRow1));
//
//    // Get table row using valid ID & check if it was updated
//    auto events = eventsdb.events.getById(5);
//    REQUIRE(events.title == testRow1.title);
//
//    // Get table row using invalid ID(should return empty eventsdb.eventsRow)
//    auto eventsFailed = eventsdb.events.getById(100);
//    REQUIRE(eventsFailed.title == "");
//
//    // Get table row using invalid ID(should return empty eventsdb.eventsRow)
//    auto eventsFailed = eventsdb.events.getById(-1);
//    REQUIRE(eventsFailed.title == "");
//
//    // Get table rows using valid offset/limit parameters
//    auto retOffsetLimit = eventsdb.events.getLimitOffset(0, 5);
//    REQUIRE(retOffsetLimit.size() == 5);
//
//    // Get table rows using valid offset/limit parameters and specific field's ID
//    REQUIRE(eventsdb.events.getLimitOffsetByField(0, 4, EventsTableFields::Date, "0").size() == 4);
//
//    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
//    auto retOffsetLimitBigger = eventsdb.events.getLimitOffset(0, 100);
//    REQUIRE(retOffsetLimitBigger.size() == 4);
//
//    // Get table rows using invalid offset/limit parameters(should return empty object)
//    auto retOffsetLimitFailed = eventsdb.events.getLimitOffset(5, 4);
//    REQUIRE(retOffsetLimitFailed.size() == 0);
//
//    REQUIRE(eventsdb.events.removeById(2));
//
//    // All records should fit in this date time period
//    auto retFiltered = selectByDatePeriod(2007242323, 2401251353);
//    REQUIRE(retOffsetLimitBigger.size() == 4);
//
//    // Should return empty vector
//    auto retFiltered = selectByDatePeriod(2007242323, 2401251353);
//    REQUIRE(retOffsetLimitBigger.size() == 0);
//
//    // Table should have now 4 elements
//    REQUIRE(eventsdb.events.count() == 4);
//
//    // Get table row using invalid ID(should return empty eventsdb.eventsRow)
//    auto eventsFailed = eventsdb.events.getById(5);
//    REQUIRE(eventsFailed.title == "");
//
//    // Remove non existing element
//    REQUIRE(eventsdb.events.removeById(100));
//
//    // Remove all elements from table
//    REQUIRE(eventsdb.events.removeById(1));
//    REQUIRE(eventsdb.events.removeById(3));
//    REQUIRE(eventsdb.events.removeById(4));
//    REQUIRE(eventsdb.events.removeById(5));
//
//    // Table should be empty now
//    REQUIRE(eventsdb.events.count() == 0);
//
//    Database::deinitialize();
//}