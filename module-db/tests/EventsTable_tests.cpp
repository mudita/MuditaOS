#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/EventsDB.hpp"

#include "Tables/EventsTable.hpp"

#include <vfs.hpp>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>

TEST_CASE("Events Table tests")
{
    Database::initialize();

    vfs.remove(EventsDB::GetDBName());

    EventsDB eventsDb;
    REQUIRE(eventsDb.isInitialized());

    auto &eventsTbl = eventsDb.events;
    REQUIRE(eventsTbl.count() == 0);

    SECTION("Default Constructor")
    {
        EventsTableRow testRow;
        REQUIRE(testRow.ID == DB_ID_NONE);
        REQUIRE(testRow.title == "");
        REQUIRE(testRow.description == "");
        REQUIRE(testRow.date_from == 0);
        REQUIRE(testRow.date_till == 0);
        REQUIRE(testRow.reminder == 0);
        REQUIRE(testRow.repeat == 0);
        REQUIRE(testRow.time_zone == 0);
        REQUIRE_FALSE(testRow.isValid());
    }

    REQUIRE(eventsTbl.add({{.ID = 0},
                           .title       = "Event3",
                           .description = "Desc3",
                           .date_from   = 1910201424,
                           .date_till   = 1910201536,
                           .reminder    = 1,
                           .repeat      = 2,
                           .time_zone   = 1}));
    REQUIRE(eventsTbl.add({{.ID = 0},
                           .title       = "Event4",
                           .description = "Desc4",
                           .date_from   = 2104191224,
                           .date_till   = 2104201536,
                           .reminder    = 0,
                           .repeat      = 3,
                           .time_zone   = 0}));

    REQUIRE(eventsTbl.count() == 2);

    SECTION("Get entry by ID")
    {
        auto entry = eventsTbl.getById(2);
        REQUIRE(entry.ID == 2);
        REQUIRE(entry.title == "Event4");
        REQUIRE(entry.description == "Desc4");
        REQUIRE(entry.date_from == 2104191224);
        REQUIRE(entry.date_till == 2104201536);
        REQUIRE(entry.reminder == 0);
        REQUIRE(entry.repeat == 3);
        REQUIRE(entry.time_zone == 0);
        REQUIRE(entry.isValid());
    }

    SECTION("Remove entry by ID")
    {
        auto response = eventsTbl.removeById(2);
        REQUIRE(response);
        REQUIRE_FALSE(eventsTbl.count() == 2);
        REQUIRE_NOTHROW(eventsTbl.getById(2));
        auto entry = eventsTbl.getById(1);
        REQUIRE(entry.ID == 1);
        REQUIRE(entry.title == "Event3");
        REQUIRE(entry.description == "Desc3");
        REQUIRE(entry.date_from == 1910201424);
        REQUIRE(entry.date_till == 1910201536);
        REQUIRE(entry.reminder == 1);
        REQUIRE(entry.repeat == 2);
        REQUIRE(entry.time_zone == 1);
        REQUIRE(entry.isValid());
    }

    SECTION("Update entry by ID")
    {
        auto entry =
            EventsTableRow({{.ID = 2}, "TestUpdateEvent", "Tested update event", 1910201500, 1910201854, 0, 2, 1});
        REQUIRE(eventsTbl.update(entry));
        auto record = eventsTbl.getById(2);
        REQUIRE(record.ID == 2);
        REQUIRE(record.title == "TestUpdateEvent");
        REQUIRE(record.description == "Tested update event");
        REQUIRE(record.date_from == 1910201500);
        REQUIRE(record.date_till == 1910201854);
        REQUIRE(record.reminder == 0);
        REQUIRE(record.repeat == 2);
        REQUIRE(record.time_zone == 1);
        REQUIRE(record.isValid());
    }

    SECTION("Select entry by date")
    {
        REQUIRE(eventsTbl.add({{.ID = 0},
                               .title       = "Event5",
                               .description = "Desc5",
                               .date_from   = 1910201424,
                               .date_till   = 1910201536,
                               .reminder    = 1,
                               .repeat      = 2,
                               .time_zone   = 1}));
        REQUIRE(eventsTbl.add({{.ID = 0},
                               .title       = "Event6",
                               .description = "Desc6",
                               .date_from   = 2104191224,
                               .date_till   = 2104201536,
                               .reminder    = 0,
                               .repeat      = 3,
                               .time_zone   = 0}));
        REQUIRE(eventsTbl.add({{.ID = 0},
                               .title       = "Event7",
                               .description = "Desc7",
                               .date_from   = 1910201524,
                               .date_till   = 1910201536,
                               .reminder    = 1,
                               .repeat      = 2,
                               .time_zone   = 1}));
        REQUIRE(eventsTbl.add({{.ID = 0},
                               .title       = "Event8",
                               .description = "Desc8",
                               .date_from   = 1504191224,
                               .date_till   = 1504201536,
                               .reminder    = 0,
                               .repeat      = 3,
                               .time_zone   = 0}));
        REQUIRE(eventsTbl.add({{.ID = 0},
                               .title       = "Event9",
                               .description = "Desc9",
                               .date_from   = 2510201424,
                               .date_till   = 2510201536,
                               .reminder    = 1,
                               .repeat      = 2,
                               .time_zone   = 1}));
        REQUIRE(eventsTbl.add({{.ID = 0},
                               .title       = "Event10",
                               .description = "Desc10",
                               .date_from   = 2112191224,
                               .date_till   = 2112201536,
                               .reminder    = 0,
                               .repeat      = 3,
                               .time_zone   = 0}));

        auto entries = eventsTbl.selectByDatePeriod(1910201500, 2104201500);
        auto entry   = entries.back();
        REQUIRE(entry.ID == 5);
        REQUIRE(entry.title == "Event7");
        REQUIRE(entry.description == "Desc7");
        REQUIRE(entry.date_from == 1910201524);
        REQUIRE(entry.date_till == 1910201536);
        REQUIRE(entry.reminder == 1);
        REQUIRE(entry.repeat == 2);
        REQUIRE(entry.time_zone == 1);
        REQUIRE(entry.isValid());
    }

    Database::deinitialize();
}
