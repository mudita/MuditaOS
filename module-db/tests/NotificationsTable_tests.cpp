#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/NotificationsDB.hpp"

#include "Tables/NotificationsTable.hpp"

#include <vfs.hpp>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>

TEST_CASE("Notifications Table tests")
{
    Database::initialize();

    vfs.remove(NotificationsDB::GetDBName());

    NotificationsDB notificationsDb;
    REQUIRE(notificationsDb.IsInitialized());

    auto &notificationsTbl = notificationsDb.notifications;
    REQUIRE(notificationsTbl.GetCount() == 2); // it already got some entries Calls(1) and Sms(2)

    SECTION("Default Constructor")
    {
        NotificationsTableRow testRow;
        REQUIRE(testRow.ID == DB_ID_NONE);
        REQUIRE(testRow.key == 0);
        REQUIRE(testRow.value == 0);
        REQUIRE_FALSE(testRow.isValid());
    }

    REQUIRE(notificationsTbl.Add({{.ID = 0}, .key = 3, .value = 8}));
    REQUIRE(notificationsTbl.Add({{.ID = 0}, .key = 4, .value = 16}));

    REQUIRE(notificationsTbl.GetCount() == 4);

    SECTION("Get entry by ID")
    {
        auto entry = notificationsTbl.GetByID(4);
        REQUIRE(entry.ID == 4);
        REQUIRE(entry.key == 4);
        REQUIRE(entry.value == 16);
        REQUIRE(entry.isValid());
    }

    SECTION("Get by key")
    {
        auto entry = notificationsTbl.GetByID(3);
        REQUIRE(entry.ID == 3);
        REQUIRE(entry.key == 3);
        REQUIRE(entry.value == 8);
        REQUIRE(entry.isValid());
    }

    SECTION("Get table rows")
    {
        auto retOffsetLimitSmaller = notificationsTbl.GetLimitOffset(0, 2);
        REQUIRE(retOffsetLimitSmaller.size() == 2);

        auto retOffsetLimit = notificationsTbl.GetLimitOffset(0, 4);
        REQUIRE(retOffsetLimit.size() == 4);

        auto retOffsetLimitBigger = notificationsTbl.GetLimitOffset(0, 100);
        REQUIRE(retOffsetLimitBigger.size() == 4);

        auto retOffsetLimitFailed = notificationsTbl.GetLimitOffset(5, 4);
        REQUIRE(retOffsetLimitFailed.size() == 0);
    }

    SECTION("Entry update")
    {
        REQUIRE(notificationsTbl.Update({{.ID = 3}, .key = 100, .value = 200}));
        auto entry = notificationsTbl.GetByID(3);
        REQUIRE(entry.ID == 3);
        REQUIRE(entry.key == 100);
        REQUIRE(entry.value == 200);
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = notificationsTbl.GetByID(100);
        REQUIRE_FALSE(entry.isValid());
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.key == 0);
        REQUIRE(entry.value == 0);
    }

    SECTION("Get by invalid key")
    {
        auto entry = notificationsTbl.GetByID(100);
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.key == 0);
        REQUIRE(entry.value == 0);
        REQUIRE_FALSE(entry.isValid());
    }

    SECTION("Remove entries")
    {
        REQUIRE(notificationsTbl.RemoveByID(2));
        REQUIRE(notificationsTbl.GetCount() == 3);

        // Remove non exist
        REQUIRE(notificationsTbl.RemoveByID(100));

        // Remove all elements from table
        REQUIRE(notificationsTbl.RemoveByID(1));
        REQUIRE(notificationsTbl.RemoveByID(3));
        REQUIRE(notificationsTbl.RemoveByID(4));

        // Table should be empty now
        REQUIRE(notificationsTbl.GetCount() == 0);
    }

    SECTION("Remove entries by field")
    {
        REQUIRE(notificationsTbl.RemoveByField(NotificationsTableFields::key, "2"));
        REQUIRE(notificationsTbl.GetCount() == 3);

        REQUIRE(notificationsTbl.RemoveByField(NotificationsTableFields::key, "1"));
        REQUIRE(notificationsTbl.GetCount() == 2);

        REQUIRE(notificationsTbl.RemoveByField(NotificationsTableFields::key, "1"));
        REQUIRE(notificationsTbl.GetCount() == 2); // no change

        REQUIRE(notificationsTbl.RemoveByField(NotificationsTableFields::key, "100"));
        REQUIRE(notificationsTbl.GetCount() == 2); // no change

        REQUIRE(notificationsTbl.RemoveByField(NotificationsTableFields::key, "3"));
        REQUIRE(notificationsTbl.GetCount() == 1);

        REQUIRE(notificationsTbl.RemoveByField(NotificationsTableFields::key, "4"));
        REQUIRE(notificationsTbl.GetCount() == 0);
    }

    SECTION("Check uniqueness")
    {
        REQUIRE(notificationsTbl.Add({{.ID = 0}, .key = 3, .value = 100}));
        REQUIRE(notificationsTbl.GetCount() == 4);
        auto entry = notificationsTbl.GetByID(3);
        REQUIRE(entry.ID == 3);
        REQUIRE(entry.key == 3);
        REQUIRE(entry.value == 8);
        REQUIRE(entry.isValid());
    }

    Database::deinitialize();
}
