
/*
 * @file SettingsRecord_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 11.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
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
#include "../Databases/SettingsDB.hpp"
#include "../Interface/SettingsRecord.hpp"


TEST_CASE("Settings Record tests")
{
    Database::Initialize();

    vfs.remove(SettingsDB::GetDBName());

    auto settingsDB = std::make_unique<SettingsDB>();


    SettingsRecordInterface settingsRecordInterface(settingsDB.get());

    auto settingsRecord = settingsRecordInterface.GetByID(1);

    REQUIRE(settingsRecord.dbID == 1 );
    REQUIRE(settingsRecord.timeFormat12 == 0 );
    REQUIRE(settingsRecord.timeAuto == 1 );
    REQUIRE(settingsRecord.brightnessAuto == 1 );
    REQUIRE(settingsRecord.brightnessLevel == 0 );
    REQUIRE(settingsRecord.fontSize == 0 );
    REQUIRE(settingsRecord.pinMode == SettingsPinMode ::DAYS);
    REQUIRE(settingsRecord.pinDaysLeft == 0 );
    REQUIRE(settingsRecord.pin1 == "" );
    REQUIRE(settingsRecord.pin2 == "" );
    REQUIRE(settingsRecord.activeSIM == 1 );
    REQUIRE(settingsRecord.networkOperator == "" );
    REQUIRE(settingsRecord.lockPassHash == 0 );
    REQUIRE(settingsRecord.language == SettingsLanguage ::ENGLISH);

    settingsRecord.networkOperator = "MuditaConnectingPeople";
    settingsRecord.fontSize=10;
    REQUIRE(settingsRecordInterface.Update(settingsRecord));

    settingsRecord = settingsRecordInterface.GetByID(1);
    REQUIRE(settingsRecord.networkOperator == "MuditaConnectingPeople");
    REQUIRE(settingsRecord.fontSize == 10);


    Database::Deinitialize();

}