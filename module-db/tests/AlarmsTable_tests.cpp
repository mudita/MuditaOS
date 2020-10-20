// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * AlarmsTable_tests.cpp
 *
 *  Created on: 16 lip 2019
 *      Author: kuba
 */
#include "vfs.hpp"

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/AlarmsDB.hpp"
#include "Tables/AlarmsTable.hpp"

#include <algorithm>

#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Alarms Table tests")
{
    Database::initialize();

    vfs.remove(AlarmsDB::GetDBName());

    AlarmsDB alarmsDb;
    REQUIRE(alarmsDb.isInitialized());

    auto alarmsRow = alarmsDb.alarms.getById(1);
    REQUIRE(alarmsRow.ID == 1);

    alarmsRow.time   = 1234;
    alarmsRow.snooze = 2345;
    alarmsRow.status = 1;
    alarmsRow.path   = "awesome.jp2";

    REQUIRE(alarmsDb.alarms.update(alarmsRow));

    auto alarmsRow2 = alarmsDb.alarms.getById(1);

    REQUIRE(alarmsRow2.time == 1234);
    REQUIRE(alarmsRow2.snooze == 2345);
    REQUIRE(alarmsRow2.status == 1);
    REQUIRE(alarmsRow2.path == "awesome.jp2");

    // add 3 elements into table
    REQUIRE(alarmsDb.alarms.add(alarmsRow));
    REQUIRE(alarmsDb.alarms.add(alarmsRow));
    REQUIRE(alarmsDb.alarms.add(alarmsRow));

    // Table should have 4 elements
    REQUIRE(alarmsDb.alarms.count() == 4);

    // update existing element in table
    alarmsRow.ID   = 4;
    alarmsRow.path = "updated";
    REQUIRE(alarmsDb.alarms.update(alarmsRow));

    // Get table row using valid ID & check if it was updated
    auto alarm = alarmsDb.alarms.getById(4);
    REQUIRE(alarm.path == alarmsRow.path);

    // Get table row using invalid ID(should return empty alarmsDb.alarmsRow)
    auto alarmFailed = alarmsDb.alarms.getById(100);
    REQUIRE(alarmFailed.path == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = alarmsDb.alarms.getLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(alarmsDb.alarms.getLimitOffsetByField(0, 4, AlarmsTableFields::Status, "1").size() == 4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = alarmsDb.alarms.getLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = alarmsDb.alarms.getLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(alarmsDb.alarms.countByFieldId("status", 1) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(alarmsDb.alarms.countByFieldId("invalid_field", 0) == 0);

    // update existing element in table
    alarmsRow.ID   = 4;
    alarmsRow.path = "updated";
    alarmsRow.time = 2222;
    REQUIRE(alarmsDb.alarms.update(alarmsRow));

    // Get record by time
    auto alarmByTime = alarmsDb.alarms.next(2000);

    REQUIRE(alarmByTime.ID == 4);
    REQUIRE(alarmByTime.time == 2222);

    // Table should have now 3 elements
    REQUIRE(alarmsDb.alarms.removeById(3));

    REQUIRE(alarmsDb.alarms.count() == 3);

    // Remove non existing element
    REQUIRE(alarmsDb.alarms.removeById(13));

    REQUIRE(alarmsDb.alarms.count() == 3);

    // Remove all elements from table
    REQUIRE(alarmsDb.alarms.removeById(1));
    REQUIRE(alarmsDb.alarms.removeById(2));
    REQUIRE(alarmsDb.alarms.removeById(4));

    // Table should be empty now
    REQUIRE(alarmsDb.alarms.count() == 0);

    Database::deinitialize();
}
