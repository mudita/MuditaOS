// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/NotificationsDB.hpp"

#include "Tables/NotificationsTable.hpp"

#include <vfs.hpp>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <purefs/filesystem_paths.hpp>

TEST_CASE("Notifications Table tests")
{
    Database::initialize();

    const auto notificationsPath = purefs::dir::getUserDiskPath() / "notifications.db";
    std::filesystem::remove(notificationsPath);

    NotificationsDB notificationsDb{notificationsPath.c_str()};
    REQUIRE(notificationsDb.isInitialized());

    auto &notificationsTbl = notificationsDb.notifications;
    REQUIRE(notificationsTbl.count() == 2); // it already got some entries Calls(1) and Sms(2)

    SECTION("Default Constructor")
    {
        NotificationsTableRow testRow;
        REQUIRE(testRow.ID == DB_ID_NONE);
        REQUIRE(testRow.key == 0);
        REQUIRE(testRow.value == 0);
        REQUIRE_FALSE(testRow.isValid());
    }

    REQUIRE(notificationsTbl.add({{.ID = 0}, .key = 3, .value = 8}));
    REQUIRE(notificationsTbl.add({{.ID = 0}, .key = 4, .value = 16}));

    REQUIRE(notificationsTbl.count() == 4);

    SECTION("Get entry by ID")
    {
        auto entry = notificationsTbl.getById(4);
        REQUIRE(entry.ID == 4);
        REQUIRE(entry.key == 4);
        REQUIRE(entry.value == 16);
        REQUIRE(entry.isValid());
    }

    SECTION("Get by key")
    {
        auto entry = notificationsTbl.getById(3);
        REQUIRE(entry.ID == 3);
        REQUIRE(entry.key == 3);
        REQUIRE(entry.value == 8);
        REQUIRE(entry.isValid());
    }

    SECTION("Get table rows")
    {
        auto retOffsetLimitSmaller = notificationsTbl.getLimitOffset(0, 2);
        REQUIRE(retOffsetLimitSmaller.size() == 2);

        auto retOffsetLimit = notificationsTbl.getLimitOffset(0, 4);
        REQUIRE(retOffsetLimit.size() == 4);

        auto retOffsetLimitBigger = notificationsTbl.getLimitOffset(0, 100);
        REQUIRE(retOffsetLimitBigger.size() == 4);

        auto retOffsetLimitFailed = notificationsTbl.getLimitOffset(5, 4);
        REQUIRE(retOffsetLimitFailed.size() == 0);
    }

    SECTION("Entry update")
    {
        REQUIRE(notificationsTbl.update({{.ID = 3}, .key = 100, .value = 200}));
        auto entry = notificationsTbl.getById(3);
        REQUIRE(entry.ID == 3);
        REQUIRE(entry.key == 100);
        REQUIRE(entry.value == 200);
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = notificationsTbl.getById(100);
        REQUIRE_FALSE(entry.isValid());
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.key == 0);
        REQUIRE(entry.value == 0);
    }

    SECTION("Get by invalid key")
    {
        auto entry = notificationsTbl.getById(100);
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.key == 0);
        REQUIRE(entry.value == 0);
        REQUIRE_FALSE(entry.isValid());
    }

    SECTION("Remove entries")
    {
        REQUIRE(notificationsTbl.removeById(2));
        REQUIRE(notificationsTbl.count() == 3);

        // Remove non exist
        REQUIRE(notificationsTbl.removeById(100));

        // Remove all elements from table
        REQUIRE(notificationsTbl.removeById(1));
        REQUIRE(notificationsTbl.removeById(3));
        REQUIRE(notificationsTbl.removeById(4));

        // Table should be empty now
        REQUIRE(notificationsTbl.count() == 0);
    }

    SECTION("Remove entries by field")
    {
        REQUIRE(notificationsTbl.removeByField(NotificationsTableFields::key, "2"));
        REQUIRE(notificationsTbl.count() == 3);

        REQUIRE(notificationsTbl.removeByField(NotificationsTableFields::key, "1"));
        REQUIRE(notificationsTbl.count() == 2);

        REQUIRE(notificationsTbl.removeByField(NotificationsTableFields::key, "1"));
        REQUIRE(notificationsTbl.count() == 2); // no change

        REQUIRE(notificationsTbl.removeByField(NotificationsTableFields::key, "100"));
        REQUIRE(notificationsTbl.count() == 2); // no change

        REQUIRE(notificationsTbl.removeByField(NotificationsTableFields::key, "3"));
        REQUIRE(notificationsTbl.count() == 1);

        REQUIRE(notificationsTbl.removeByField(NotificationsTableFields::key, "4"));
        REQUIRE(notificationsTbl.count() == 0);
    }

    SECTION("Check uniqueness")
    {
        REQUIRE(notificationsTbl.add({{.ID = 0}, .key = 3, .value = 100}));
        REQUIRE(notificationsTbl.count() == 4);
        auto entry = notificationsTbl.getById(3);
        REQUIRE(entry.ID == 3);
        REQUIRE(entry.key == 3);
        REQUIRE(entry.value == 8);
        REQUIRE(entry.isValid());
    }

    Database::deinitialize();
}
