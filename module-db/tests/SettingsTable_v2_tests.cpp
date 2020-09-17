#include "vfs.hpp"

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/SettingsDB.hpp"

#include "Tables/SettingsTable.hpp"

TEST_CASE("Settings Table version 2 tests")
{
    Database::initialize();

    vfs.remove(SettingsDB::GetDBName());

    SettingsDB settingsDb;
    REQUIRE(settingsDb.isInitialized());

    auto &settingsTable_v2 = settingsDb.settings_v2;

    SECTION("Default Constructor")
    {
        SettingsTableRow_v2 testRow;
        REQUIRE(testRow.ID == DB_ID_NONE);
        REQUIRE(testRow.path == "");
        REQUIRE(testRow.value == "");
    }

    const std::vector<std::string> testPaths{"test/path1", "test/path2", "test/path3", "test/path4"};

    for (const auto &entry : testPaths) {
        SettingsTableRow_v2 testRow = {{.ID = 0}, .path = entry, .value = "abc"};

        REQUIRE(settingsTable_v2.add(testRow));
    }

    SECTION("Get entry - invalid ID")
    {
        auto settingsEntry = settingsTable_v2.getById(100);
        REQUIRE(settingsEntry.ID == DB_ID_NONE);
        REQUIRE(settingsEntry.path == "");
        REQUIRE(settingsEntry.value == "");
    }

    SECTION("Remove entries")
    {
        const auto &cnt = settingsTable_v2.count();

        SECTION("Remove by ID")
        {
            // Table should have now cnt - 1 elements
            REQUIRE(settingsTable_v2.removeById(1));
            REQUIRE(settingsTable_v2.count() == cnt - 1);

            // Remove non existing element
            REQUIRE(settingsTable_v2.removeById(100));

            // Table should have now cnt - 2 elements
            REQUIRE(settingsTable_v2.removeById(2));
            REQUIRE(settingsTable_v2.count() == cnt - 2);
        }

        SECTION("Remove by Field")
        {
            // Table should have now cnt - 1 elements
            REQUIRE(settingsTable_v2.removeByField(SettingsTableFields_v2::Path, "test/path1"));
            REQUIRE(settingsTable_v2.count() == cnt - 1);

            // Table should have now 0 elements
            REQUIRE(settingsTable_v2.removeByField(SettingsTableFields_v2::Value, "abc"));
            REQUIRE(settingsTable_v2.count() == 0);
        }
    }

    SECTION("Entry update")
    {
        auto settingsPre  = settingsTable_v2.getById(1);
        settingsPre.path  = "Test/test";
        settingsPre.value = "TestValue";

        REQUIRE(settingsTable_v2.update(settingsPre));

        auto settingsPost = settingsTable_v2.getById(1);
        REQUIRE(settingsPost.path == settingsPre.path);
        REQUIRE(settingsPost.value == settingsPre.value);
    }

    Database::deinitialize();
}
