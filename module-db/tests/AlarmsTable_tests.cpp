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

	alarmsRow.ID = 4;
	alarmsRow.path = "updated";
	REQUIRE(alarmsDb.alarms.Update(alarmsRow));

	auto alarm = alarmsDb.alarms.GetByID(4);
	REQUIRE( alarm.path == alarmsRow.path);

	auto alarmFailed = alarmsDb.alarms.GetByID(100);
	REQUIRE( alarmFailed.path == "" );

	auto retOffsetLimit = alarmsDb.alarms.GetLimitOffset(0,4);
	REQUIRE(retOffsetLimit.size() == 4);

	REQUIRE(alarmsDb.alarms.GetLimitOffsetByField(0,4,AlarmsTableFields::Status, "1").size() == 4);

	auto retOffsetLimitBigger = alarmsDb.alarms.GetLimitOffset(0,100);
	REQUIRE(retOffsetLimitBigger.size() == 4);

	auto retOffsetLimitFailed = alarmsDb.alarms.GetLimitOffset(5,4);
	REQUIRE(retOffsetLimitFailed.size() == 0);

	REQUIRE(alarmsDb.alarms.GetCountByFieldID("status", 1) == 4);

	REQUIRE(alarmsDb.alarms.GetCountByFieldID("invalid_field", 0) == 0);

	REQUIRE(alarmsDb.alarms.RemoveByID(3));

	REQUIRE(alarmsDb.alarms.GetCount() == 3);

	REQUIRE(alarmsDb.alarms.RemoveByID(13));

	REQUIRE(alarmsDb.alarms.GetCount() == 3);

	REQUIRE(alarmsDb.alarms.RemoveByID(1));
	REQUIRE(alarmsDb.alarms.RemoveByID(2));
	REQUIRE(alarmsDb.alarms.RemoveByID(4));

	REQUIRE(alarmsDb.alarms.GetCount() == 0);

	Database::Deinitialize();

}



