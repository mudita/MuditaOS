/*
 * AlarmsTable_tests.cpp
 *
 *  Created on: 16 lip 2019
 *      Author: kuba
 */
#include "vfs.hpp"

#include "catch.hpp"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <iostream>

#include "../Database/Database.hpp"
#include "../Databases/AlarmsDB.hpp"

#include "Tables/AlarmsTable.hpp"

TEST_CASE("Alarms Table tests")
{
	Database::Initialize();

	vfs.remove(AlarmsDB::GetDBName());

	AlarmsDB alarmsDb;

	auto alarmsRow = alarmsDb.alarms.GetByID(1);
	REQUIRE(alarmsRow.ID == 1);

	alarmsRow.time = 1234;
	alarmsRow.snooze = 2345;
	alarmsRow.status = 1;
	alarmsRow.path = "awesome.jp2";

	REQUIRE(alarmsDb.alarms.Update(alarmsRow));

	auto alarmsRow2 = alarmsDb.alarms.GetByID(1);

	REQUIRE(alarmsRow2.time == 1234);
	REQUIRE(alarmsRow2.snooze == 2345);
	REQUIRE(alarmsRow2.status == 1);
	REQUIRE(alarmsRow2.path == "awesome.jp2");

	// Add 3 elements into table
	REQUIRE(alarmsDb.alarms.Add(alarmsRow));
	REQUIRE(alarmsDb.alarms.Add(alarmsRow));
	REQUIRE(alarmsDb.alarms.Add(alarmsRow));

	// Table should have 4 elements
	REQUIRE(alarmsDb.alarms.GetCount() == 4);

	 // Update existing element in table
	alarmsRow.ID = 4;
	alarmsRow.path = "updated";
	REQUIRE(alarmsDb.alarms.Update(alarmsRow));

    // Get table row using valid ID & check if it was updated
	auto alarm = alarmsDb.alarms.GetByID(4);
	REQUIRE( alarm.path == alarmsRow.path);

	// Get table row using invalid ID(should return empty alarmsDb.alarmsRow)
	auto alarmFailed = alarmsDb.alarms.GetByID(100);
	REQUIRE( alarmFailed.path == "" );

	// Get table rows using valid offset/limit parameters
	auto retOffsetLimit = alarmsDb.alarms.GetLimitOffset(0,4);
	REQUIRE(retOffsetLimit.size() == 4);

	// Get table rows using valid offset/limit parameters and specific field's ID
	REQUIRE(alarmsDb.alarms.GetLimitOffsetByField(0,4,AlarmsTableFields::Status, "1").size() == 4);

	// Get table rows using invalid limit parameters(should return 4 elements instead of 100)
	auto retOffsetLimitBigger = alarmsDb.alarms.GetLimitOffset(0,100);
	REQUIRE(retOffsetLimitBigger.size() == 4);

	// Get table rows using invalid offset/limit parameters(should return empty object)
	auto retOffsetLimitFailed = alarmsDb.alarms.GetLimitOffset(5,4);
	REQUIRE(retOffsetLimitFailed.size() == 0);

	// Get count of elements by field's ID
	REQUIRE(alarmsDb.alarms.GetCountByFieldID("status", 1) == 4);

	// Get count of elements by invalid field's ID
	REQUIRE(alarmsDb.alarms.GetCountByFieldID("invalid_field", 0) == 0);

	// Update existing element in table
	alarmsRow.ID = 4;
	alarmsRow.path = "updated";
	alarmsRow.time = 2222;
	REQUIRE(alarmsDb.alarms.Update(alarmsRow));

	// Get record by time
	auto alarmByTime = alarmsDb.alarms.GetNext(2000);

	REQUIRE(alarmByTime.ID == 4);
	REQUIRE(alarmByTime.time == 2222);

	// Table should have now 3 elements
	REQUIRE(alarmsDb.alarms.RemoveByID(3));

	REQUIRE(alarmsDb.alarms.GetCount() == 3);

	// Remove non existing element
	REQUIRE(alarmsDb.alarms.RemoveByID(13));

	REQUIRE(alarmsDb.alarms.GetCount() == 3);

	// Remove all elements from table
	REQUIRE(alarmsDb.alarms.RemoveByID(1));
	REQUIRE(alarmsDb.alarms.RemoveByID(2));
	REQUIRE(alarmsDb.alarms.RemoveByID(4));

	// Table should be empty now
	REQUIRE(alarmsDb.alarms.GetCount() == 0);

	Database::Deinitialize();

}



