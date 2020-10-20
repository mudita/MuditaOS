// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * @file SettingsTable_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "vfs.hpp"

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/SettingsDB.hpp"

#include "Tables/SettingsTable.hpp"

#include <algorithm>

#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Settings Table tests")
{
    Database::initialize();

    vfs.remove(SettingsDB::GetDBName());

    {

        SettingsDB settingsDb;
        REQUIRE(settingsDb.isInitialized());

        auto settingsRow = settingsDb.settings.getById(1);
        REQUIRE(settingsRow.ID == 1);

        settingsRow.timeFormat12   = false;
        settingsRow.timeDateFormat = false;
        settingsRow.pin1           = "4321";
        settingsRow.pin2           = "5432";
        settingsRow.language       = SettingsLanguage ::POLISH;
        REQUIRE(settingsDb.settings.update(settingsRow));

        settingsRow = settingsDb.settings.getById(1);

        REQUIRE(settingsRow.timeFormat12 == false);
        REQUIRE(settingsRow.timeDateFormat == false);
        REQUIRE(settingsRow.pin1 == "4321");
        REQUIRE(settingsRow.pin2 == "5432");
        REQUIRE(settingsRow.language == SettingsLanguage ::POLISH);
    }

    {
        SettingsDB settingsDb;

        auto settingsRow = settingsDb.settings.getById(1);

        REQUIRE(settingsRow.timeFormat12 == false);
        REQUIRE(settingsRow.timeDateFormat == false);
        REQUIRE(settingsRow.pin1 == "4321");
        REQUIRE(settingsRow.pin2 == "5432");
        REQUIRE(settingsRow.language == SettingsLanguage ::POLISH);
    }

    Database::deinitialize();
}
