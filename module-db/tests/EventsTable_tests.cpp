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
        REQUIRE(testRow.date_from == TIME_POINT_INVALID);
        REQUIRE(testRow.date_till == TIME_POINT_INVALID);
        REQUIRE(testRow.reminder == 0);
        REQUIRE(testRow.repeat == 0);
        REQUIRE_FALSE(testRow.isValid());
    }

    REQUIRE(eventsTbl.add({{.ID = 0},
                           .title     = "Event3",
                           .date_from = TimePointFromString("2019-10-20 14:24"),
                           .date_till = TimePointFromString("2019-10-20 15:36"),
                           .reminder  = 1,
                           .repeat    = 2}));
    REQUIRE(eventsTbl.add({{.ID = 0},
                           .title     = "Event4",
                           .date_from = TimePointFromString("2021-10-20 12:24"),
                           .date_till = TimePointFromString("2021-10-20 15:36"),
                           .reminder  = 0,
                           .repeat    = 3}));

    REQUIRE(eventsTbl.count() == 2);

    SECTION("Get entry by ID")
    {
        auto entry = eventsTbl.getById(2);
        REQUIRE(entry.ID == 2);
        REQUIRE(entry.title == "Event4");
        REQUIRE(entry.date_from == TimePointFromString("2021-10-20 12:24"));
        REQUIRE(entry.date_till == TimePointFromString("2021-10-20 15:36"));
        REQUIRE(entry.reminder == 0);
        REQUIRE(entry.repeat == 3);
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
        REQUIRE(entry.date_from == TimePointFromString("2019-10-20 14:24"));
        REQUIRE(entry.date_till == TimePointFromString("2019-10-20 15:36"));
        REQUIRE(entry.reminder == 1);
        REQUIRE(entry.repeat == 2);
        REQUIRE(entry.isValid());
    }

    SECTION("Update entry by ID")
    {
        auto entry = EventsTableRow({{.ID = 2},
                                     "TestUpdateEvent",
                                     TimePointFromString("2019-10-20 15:00"),
                                     TimePointFromString("2019-10-20 18:54"),
                                     0,
                                     2});
        REQUIRE(eventsTbl.update(entry));
        auto record = eventsTbl.getById(2);
        REQUIRE(record.ID == 2);
        REQUIRE(record.title == "TestUpdateEvent");
        REQUIRE(record.date_from == TimePointFromString("2019-10-20 15:00"));
        REQUIRE(record.date_till == TimePointFromString("2019-10-20 18:54"));
        REQUIRE(record.reminder == 0);
        REQUIRE(record.repeat == 2);
        REQUIRE(record.isValid());
    }

    SECTION("Select entry by date")
    {
        REQUIRE(eventsTbl.add({{.ID = 0},
                               .title     = "Event5",
                               .date_from = TimePointFromString("2019-10-20 14:24"),
                               .date_till = TimePointFromString("2019-10-20 15:36"),
                               .reminder  = 1,
                               .repeat    = 2}));
        REQUIRE(eventsTbl.add({{.ID = 1},
                               .title     = "Event6",
                               .date_from = TimePointFromString("2021-04-19 12:24"),
                               .date_till = TimePointFromString("2021-04-20 15:36"),
                               .reminder  = 0,
                               .repeat    = 3}));
        REQUIRE(eventsTbl.add({{.ID = 2},
                               .title     = "Event7",
                               .date_from = TimePointFromString("2019-10-20 15:24"),
                               .date_till = TimePointFromString("2019-10-20 15:36"),
                               .reminder  = 1,
                               .repeat    = 2}));
        REQUIRE(eventsTbl.add({{.ID = 3},
                               .title     = "Event8",
                               .date_from = TimePointFromString("2021-04-19 12:24"),
                               .date_till = TimePointFromString("2019-10-20 15:36"),
                               .reminder  = 0,
                               .repeat    = 3}));
        REQUIRE(eventsTbl.add({{.ID = 4},
                               .title     = "Event9",
                               .date_from = TimePointFromString("2025-10-20 15:24"),
                               .date_till = TimePointFromString("2025-10-20 15:36"),
                               .reminder  = 1,
                               .repeat    = 2}));
        REQUIRE(eventsTbl.add({{.ID = 5},
                               .title     = "Event10",
                               .date_from = TimePointFromString("2021-12-19 12:24"),
                               .date_till = TimePointFromString("2021-12-20 15:36"),
                               .reminder  = 0,
                               .repeat    = 3}));

        auto entries = eventsTbl.selectByDatePeriod(TimePointFromString("2000-01-01 00:00"),
                                                    TimePointFromString("2012-12-31 23:59"));

        REQUIRE(entries.size() == 0);
    }
    for (uint32_t i = 1; i <= 8; ++i) {
        auto response = eventsTbl.removeById(i);
        REQUIRE(response);
    }
    REQUIRE(eventsTbl.count() == 0);
    SECTION("Get all limited by date")
    {
        for (uint32_t i = 1; i <= 3; ++i) {
            auto response = eventsTbl.removeById(i);
            REQUIRE(response);
        }
        REQUIRE(eventsTbl.count() == 0);

        const std::array<TimePoint, 6> paramDate{TimePointFromString("2018-10-20 15:24"),
                                                 TimePointFromString("2019-10-30 14:24"),
                                                 TimePointFromString("2020-10-20 14:24"),
                                                 TimePointFromString("2021-12-20 14:24"),
                                                 TimePointFromString("2022-10-20 14:24"),
                                                 TimePointFromString("2023-10-20 14:24")};
        const std::array<uint32_t, 6> paramID{3, 4, 5, 6, 7, 8};
        const std::array<std::string, 6> paramName{"Event1", "Event2", "Event3", "Event4", "Event5", "Event6"};
        REQUIRE(eventsTbl.add({{.ID = 1},
                               .title     = "Event1",
                               .date_from = paramDate[0],
                               .date_till = TimePointFromString("2030-10-20 15:24"),
                               .reminder  = 0,
                               .repeat    = 0}));
        REQUIRE(eventsTbl.add({{.ID = 2},
                               .title     = "Event2",
                               .date_from = paramDate[5],
                               .date_till = TimePointFromString("2030-10-20 15:24"),
                               .reminder  = 0,
                               .repeat    = 0}));
        REQUIRE(eventsTbl.add({{.ID = 3},
                               .title     = "Event3",
                               .date_from = paramDate[2],
                               .date_till = TimePointFromString("2030-10-20 15:24"),
                               .reminder  = 0,
                               .repeat    = 0}));
        REQUIRE(eventsTbl.add({{.ID = 4},
                               .title     = "Event4",
                               .date_from = paramDate[3],
                               .date_till = TimePointFromString("2030-10-20 15:24"),
                               .reminder  = 0,
                               .repeat    = 0}));
        REQUIRE(eventsTbl.add({{.ID = 5},
                               .title     = "Event5",
                               .date_from = paramDate[4],
                               .date_till = TimePointFromString("2030-10-20 15:24"),
                               .reminder  = 0,
                               .repeat    = 0}));
        REQUIRE(eventsTbl.add({{.ID = 6},
                               .title     = "Event6",
                               .date_from = paramDate[1],
                               .date_till = TimePointFromString("2030-10-20 15:24"),
                               .reminder  = 0,
                               .repeat    = 0}));

        auto entries = eventsTbl.getLimitOffsetByDate(0, 6);
        REQUIRE(entries.size() == 6);
        uint32_t index = 0;
        for (auto entry : entries) {
            REQUIRE(entry.ID == paramID[index]);
            REQUIRE(entry.title == paramName[index]);
            REQUIRE(entry.date_from == paramDate[index]);
            REQUIRE(entry.date_till == TimePointFromString("2030-10-20 15:24"));
            REQUIRE(entry.reminder == 0);
            REQUIRE(entry.repeat == 0);
            REQUIRE(entry.isValid());
            ++index;
        }
    }
    Database::deinitialize();
}
