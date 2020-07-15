/*
 * AlarmsRecord_tests.cpp
 *
 *  Created on: 16 lip 2019
 *      Author: kuba
 */

#include "vfs.hpp"

#include <catch2/catch.hpp>

#include <Interface/ContactRecord.hpp>

#include "Database/Database.hpp"
#include "Databases/AlarmsDB.hpp"
#include "Interface/AlarmsRecord.hpp"

#include <algorithm>

#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Alarms Record tests")
{
    Database::initialize();

    vfs.remove(AlarmsDB::GetDBName());

    auto alarmsDB = std::make_unique<AlarmsDB>();
    REQUIRE(alarmsDB->IsInitialized());

    AlarmsRecordInterface alarmsRecordInterface(alarmsDB.get());

    auto alarmsRecord = alarmsRecordInterface.GetByID(1);

    REQUIRE(alarmsRecord.ID == 1);
    REQUIRE(alarmsRecord.time == 0);
    REQUIRE(alarmsRecord.snooze == 0);
    REQUIRE(alarmsRecord.status == 0);
    REQUIRE(alarmsRecord.path == "");

    alarmsRecord.time = 1234;
    alarmsRecord.path = "awesome.mp3";
    REQUIRE(alarmsRecordInterface.Update(alarmsRecord));

    alarmsRecord = alarmsRecordInterface.GetByID(1);

    REQUIRE(alarmsRecord.time == 1234);
    REQUIRE(alarmsRecord.path == "awesome.mp3");

    Database::deinitialize();
}
