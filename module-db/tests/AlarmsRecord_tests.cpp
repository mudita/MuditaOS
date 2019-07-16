/*
 * AlarmsRecord_tests.cpp
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
#include <Interface/ContactRecord.hpp>

#include "../Database/Database.hpp"
#include "../Databases/AlarmsDB.hpp"
#include "../Interface/AlarmsRecord.hpp"


TEST_CASE("Alarms Record tests")
{
	Database::Initialize();

	vfs.remove(AlarmsDB::GetDBName());

	auto alarmsDB = std::make_unique<AlarmsDB>();

	AlarmsRecordInterface alarmsRecordInterface(alarmsDB.get());

	auto alarmsRecord = alarmsRecordInterface.GetByID(1);

	REQUIRE(alarmsRecord.ID == 1 );
	REQUIRE(alarmsRecord.time == 0 );
	REQUIRE(alarmsRecord.snooze == 0 );
	REQUIRE(alarmsRecord.status == 0 );
	REQUIRE(alarmsRecord.path == "" );

	alarmsRecord.time = 1234;
	alarmsRecord.path = "awesome.mp3";
	REQUIRE(alarmsRecordInterface.Update(alarmsRecord));

	alarmsRecord = alarmsRecordInterface.GetByID(1);

	REQUIRE(alarmsRecord.time == 1234);
	REQUIRE(alarmsRecord.path == "awesome.mp3");

	Database::Deinitialize();
}




