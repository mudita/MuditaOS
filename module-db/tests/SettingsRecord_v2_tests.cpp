#include "vfs.hpp"

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/SettingsDB.hpp"
#include "Interface/SettingsRecord_v2.hpp"

#include <algorithm>

#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Settings Record version 2 tests")
{
    Database::initialize();

    vfs.remove(SettingsDB::GetDBName());

    auto settingsDB = std::make_unique<SettingsDB>();
    REQUIRE(settingsDB->isInitialized());

    SECTION("Default Constructor")
    {
        SettingsRecord_v2 testRec;
        REQUIRE_FALSE(testRec.isValid());
        REQUIRE(testRec.ID == DB_ID_NONE);
        REQUIRE(testRec.getPath() == "");
        REQUIRE(testRec.getValue<std::string>() == "");
    }

    SECTION("Constructor from NotificationsTableRow")
    {
        SettingsTableRow_v2 tableRow{{.ID = 50}, .path = "test/test", .value = "test"};

        SettingsRecord_v2 testRec(tableRow);
        REQUIRE(testRec.isValid());
        REQUIRE(testRec.ID == 50);
        REQUIRE(testRec.getPath() == "test/test");
        REQUIRE(testRec.getValue<std::string>() == "test");
    }

    SettingsRecordInterface_v2 settingsRecordInterface(settingsDB.get());

    SECTION("Add entries")
    {
        const std::vector<SettingsTableRow_v2> rows = {{{.ID = 1}, .path = "test1/test", .value = "1"},
                                                       {{.ID = 2}, .path = "test2/test", .value = "2"}};

        for (const auto &row : rows) {
            REQUIRE(settingsRecordInterface.Add(SettingsRecord_v2(row)));
        }

        REQUIRE(settingsRecordInterface.GetCount() == rows.size());
    }

    SECTION("Get entry")
    {
        SettingsTableRow_v2 tableRow{{.ID = 1}, .path = "audio/RoutingSpeakerphone/volume", .value = "2"};
        REQUIRE(settingsRecordInterface.Add(SettingsRecord_v2(tableRow)));

        SECTION("Get by ID")
        {
            auto settingsEntry = settingsRecordInterface.GetByID(1);
            REQUIRE(settingsEntry.ID == 1);
            REQUIRE(settingsEntry.getPath() == "audio/RoutingSpeakerphone/volume");
            REQUIRE(settingsEntry.getValue<int>() == 2);
        }

        SECTION("Get by path")
        {
            auto settingsEntry = settingsRecordInterface.GetByPath("audio/RoutingSpeakerphone/volume");
            REQUIRE(settingsEntry.ID == 1);
            REQUIRE(settingsEntry.getPath() == "audio/RoutingSpeakerphone/volume");
            REQUIRE(settingsEntry.getValue<int>() == 2);
        }
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = settingsRecordInterface.GetByID(100);
        REQUIRE_FALSE(entry.isValid());
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.getPath() == "");
        REQUIRE(entry.getValue<std::string>() == "");
    }

    SECTION("Get entry - invalid path")
    {
        auto entry = settingsRecordInterface.GetByPath("abc");
        REQUIRE_FALSE(entry.isValid());
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.getPath() == "");
        REQUIRE(entry.getValue<std::string>() == "");
    }

    SECTION("Create, update, delete")
    {
        const std::vector<SettingsTableRow_v2> rows = {{{.ID = 1}, .path = "test1/test", .value = "1"},
                                                       {{.ID = 2}, .path = "test2/test", .value = "2"}};

        for (const auto &row : rows) {
            REQUIRE(settingsRecordInterface.Add(SettingsRecord_v2(row)));
        }

        REQUIRE(settingsRecordInterface.GetCount() == rows.size());

        SECTION("Update entry")
        {
            SettingsRecord_v2 entryToUpdate{{{.ID = 1}, .path = "pure/test", .value = "test"}};
            REQUIRE(settingsRecordInterface.Update(entryToUpdate));
            auto updatedEntry = settingsRecordInterface.GetByID(1);
            REQUIRE(updatedEntry.isValid());
            REQUIRE(updatedEntry.ID == entryToUpdate.ID);
            REQUIRE(updatedEntry.getPath() == entryToUpdate.getPath());
            REQUIRE(updatedEntry.getValue<std::string>() == entryToUpdate.getValue<std::string>());
        }

        SECTION("Clear table")
        {
            REQUIRE(settingsRecordInterface.RemoveByID(1));
            REQUIRE(settingsRecordInterface.RemoveByID(2));

            REQUIRE(settingsRecordInterface.GetCount() == 0);
        }
    }

    Database::deinitialize();
}
