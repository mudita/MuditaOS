
/*
 * @file SettingsTable_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
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

#include "../Database/Database.hpp"
#include "../Databases/SettingsDB.hpp"

#include "Tables/SettingsTable.hpp"

TEST_CASE("Settings Table tests")
{
    Database::Initialize();

    vfs.remove(SettingsDB::GetDBName());

    {

        SettingsDB settingsDb;

        auto settingsRow = settingsDb.settings.GetByID(1);
        REQUIRE(settingsRow.ID == 1);

        settingsRow.pin1 = "4321";
        settingsRow.pin2 = "5432";
        settingsRow.language = SettingsLanguage ::POLISH;
        REQUIRE(settingsDb.settings.Update(settingsRow));

        settingsRow = settingsDb.settings.GetByID(1);

        REQUIRE(settingsRow.pin1 == "4321");
        REQUIRE(settingsRow.pin2 == "5432");
        REQUIRE(settingsRow.language == SettingsLanguage ::POLISH);
    }

    {
        SettingsDB settingsDb;

        auto settingsRow = settingsDb.settings.GetByID(1);

        REQUIRE(settingsRow.pin1 == "4321");
        REQUIRE(settingsRow.pin2 == "5432");
        REQUIRE(settingsRow.language == SettingsLanguage ::POLISH);
    }



    Database::Deinitialize();
}