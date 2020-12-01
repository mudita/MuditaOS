// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "vfs.hpp"

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/SettingsDB.hpp"
#include "Interface/SettingsRecord.hpp"
#include "Interface/ContactRecord.hpp"

#include <algorithm>

#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Settings Record tests")
{
    Database::initialize();

    vfs.remove(SettingsDB::GetDBName());

    auto settingsDB = std::make_unique<SettingsDB>();
    REQUIRE(settingsDB->isInitialized());

    SettingsRecordInterface settingsRecordInterface(settingsDB.get());

    auto settingsRecord = settingsRecordInterface.GetByID(1);

    REQUIRE(settingsRecord.dbID == 1);
    REQUIRE(settingsRecord.timeFormat12 == 0);
    REQUIRE(settingsRecord.timeAuto == 1);
    REQUIRE(settingsRecord.brightnessAuto == 1);
    REQUIRE(settingsRecord.brightnessLevel == 0);
    REQUIRE(settingsRecord.fontSize == 0);
    REQUIRE(settingsRecord.pinMode == SettingsPinMode ::DAYS);
    REQUIRE(settingsRecord.pinDaysLeft == 0);
    REQUIRE(settingsRecord.pin1 == "");
    REQUIRE(settingsRecord.pin2 == "");
    REQUIRE(settingsRecord.activeSIM == SettingsRecord::ActiveSim::SIM1);
    REQUIRE(settingsRecord.networkOperator == "");
    REQUIRE(settingsRecord.lockPassHash == 0);
    REQUIRE(settingsRecord.displayLanguage == "English");
    REQUIRE(settingsRecord.inputLanguage == "English");

    settingsRecord.networkOperator = "MuditaConnectingPeople";
    settingsRecord.fontSize        = 10;
    REQUIRE(settingsRecordInterface.Update(settingsRecord));

    settingsRecord = settingsRecordInterface.GetByID(1);
    REQUIRE(settingsRecord.networkOperator == "MuditaConnectingPeople");
    REQUIRE(settingsRecord.fontSize == 10);

    Database::deinitialize();
}
