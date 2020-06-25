#include <catch2/catch.hpp>

#include "Interface/NotificationsRecord.hpp"
#include "Database/Database.hpp"
#include "Databases/NotificationsDB.hpp"

#include <vfs.hpp>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>

TEST_CASE("Notifications Record tests")
{
    Database::Initialize();

    vfs.remove(NotificationsDB::GetDBName());

    NotificationsDB notificationsDb;

    REQUIRE(notificationsDb.IsInitialized());

    SECTION("Default Constructor")
    {
        NotificationsRecord testRec;
        REQUIRE_FALSE(testRec.isValidRecord());
        REQUIRE(testRec.ID == DB_ID_NONE);
        REQUIRE(testRec.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(testRec.value == 0);
    }

    SECTION("Constructor from NotificationsTableRow")
    {
        NotificationsTableRow tableRow{
            {.ID = 10}, .key = static_cast<uint32_t>(NotificationsRecord::Key::Calls), .value = 2};

        NotificationsRecord testRec(tableRow);
        REQUIRE(testRec.isValidRecord());
        REQUIRE(testRec.ID == 10);
        REQUIRE(testRec.key == NotificationsRecord::Key::Calls);
        REQUIRE(testRec.value == 2);
    }

    NotificationsRecordInterface NotificationsRecordInterface(&notificationsDb);

    NotificationsRecord testRec;

    SECTION("Get entry by ID")
    {
        auto callsNotifications = NotificationsRecordInterface.GetByID(1);
        REQUIRE(callsNotifications.isValidRecord());
        REQUIRE(callsNotifications.ID == 1);
        REQUIRE(callsNotifications.key == NotificationsRecord::Key::Calls);
        REQUIRE(callsNotifications.value == 0);

        auto smsNotifications = NotificationsRecordInterface.GetByID(2);
        REQUIRE(smsNotifications.isValidRecord());
        REQUIRE(smsNotifications.ID == 2);
        REQUIRE(smsNotifications.key == NotificationsRecord::Key::Sms);
        REQUIRE(smsNotifications.value == 0);
    }

    SECTION("Get entry by key")
    {
        auto callsNotifications = NotificationsRecordInterface.GetByKey(NotificationsRecord::Key::Calls);
        REQUIRE(callsNotifications.isValidRecord());
        REQUIRE(callsNotifications.ID == 1);
        REQUIRE(callsNotifications.key == NotificationsRecord::Key::Calls);
        REQUIRE(callsNotifications.value == 0);

        auto smsNotifications = NotificationsRecordInterface.GetByKey(NotificationsRecord::Key::Sms);
        REQUIRE(smsNotifications.isValidRecord());
        REQUIRE(smsNotifications.ID == 2);
        REQUIRE(smsNotifications.key == NotificationsRecord::Key::Sms);
        REQUIRE(smsNotifications.value == 0);
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = NotificationsRecordInterface.GetByID(100);
        REQUIRE_FALSE(entry.isValidRecord());
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(entry.value == 0);
    }

    SECTION("Get entry by invalid key")
    {
        auto callsNotifications = NotificationsRecordInterface.GetByKey(NotificationsRecord::Key::NotValidKey);
        REQUIRE_FALSE(callsNotifications.isValidRecord());
        REQUIRE(callsNotifications.ID == DB_ID_NONE);
        REQUIRE(callsNotifications.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(callsNotifications.value == 0);

        auto smsNotifications = NotificationsRecordInterface.GetByKey(NotificationsRecord::Key::NumberOfKeys);
        REQUIRE_FALSE(smsNotifications.isValidRecord());
        REQUIRE(smsNotifications.ID == DB_ID_NONE);
        REQUIRE(smsNotifications.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(smsNotifications.value == 0);
    }

    SECTION("Entry update value")
    {
        auto entryPre  = NotificationsRecordInterface.GetByID(1);
        entryPre.value = entryPre.value + 100;

        REQUIRE(NotificationsRecordInterface.Update(entryPre));

        auto entryPost = NotificationsRecordInterface.GetByID(1);
        REQUIRE(entryPost.ID == entryPre.ID);
        REQUIRE(entryPost.key == entryPre.key);
        REQUIRE(entryPost.value == entryPre.value);
    }

    SECTION("Entry update key")
    {
        auto entryPre = NotificationsRecordInterface.GetByID(1);
        entryPre.key  = NotificationsRecord::Key::Sms;

        REQUIRE_FALSE(NotificationsRecordInterface.Update(entryPre));

        auto entryPost = NotificationsRecordInterface.GetByID(1);
        REQUIRE(entryPost.ID == entryPre.ID);
        REQUIRE_FALSE(entryPost.key == entryPre.key);
        REQUIRE(entryPost.value == entryPre.value);
    }

    Database::Deinitialize();
}
