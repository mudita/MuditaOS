#include <catch2/catch.hpp>

#include "Interface/NotificationsRecord.hpp"
#include "Database/Database.hpp"
#include "Databases/NotificationsDB.hpp"
#include "module-db/queries/notifications/QueryNotificationsGet.hpp"
#include "module-db/queries/notifications/QueryNotificationsIncrement.hpp"
#include "module-db/queries/notifications/QueryNotificationsClear.hpp"
#include "module-db/queries/notifications/QueryNotificationsGetAll.hpp"

#include <vfs.hpp>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>

TEST_CASE("Notifications Record tests")
{
    Database::initialize();

    vfs.remove(NotificationsDB::GetDBName());

    NotificationsDB notificationsDb;

    REQUIRE(notificationsDb.isInitialized());

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

    NotificationsRecordInterface notificationsRecordInterface(&notificationsDb);

    NotificationsRecord testRec;
    auto numberOfNotifcations = notificationsRecordInterface.GetCount();
    REQUIRE(numberOfNotifcations == 2); // calls and sms notifications

    SECTION("Get entry by ID")
    {
        auto callsNotifications = notificationsRecordInterface.GetByID(1);
        REQUIRE(callsNotifications.isValidRecord());
        REQUIRE(callsNotifications.ID == 1);
        REQUIRE(callsNotifications.key == NotificationsRecord::Key::Calls);
        REQUIRE(callsNotifications.value == 0);

        auto smsNotifications = notificationsRecordInterface.GetByID(2);
        REQUIRE(smsNotifications.isValidRecord());
        REQUIRE(smsNotifications.ID == 2);
        REQUIRE(smsNotifications.key == NotificationsRecord::Key::Sms);
        REQUIRE(smsNotifications.value == 0);
    }

    SECTION("Get entry by key")
    {
        auto callsNotifications = notificationsRecordInterface.GetByKey(NotificationsRecord::Key::Calls);
        REQUIRE(callsNotifications.isValidRecord());
        REQUIRE(callsNotifications.ID == 1);
        REQUIRE(callsNotifications.key == NotificationsRecord::Key::Calls);
        REQUIRE(callsNotifications.value == 0);

        auto smsNotifications = notificationsRecordInterface.GetByKey(NotificationsRecord::Key::Sms);
        REQUIRE(smsNotifications.isValidRecord());
        REQUIRE(smsNotifications.ID == 2);
        REQUIRE(smsNotifications.key == NotificationsRecord::Key::Sms);
        REQUIRE(smsNotifications.value == 0);
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = notificationsRecordInterface.GetByID(100);
        REQUIRE_FALSE(entry.isValidRecord());
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(entry.value == 0);
    }

    SECTION("Get entry by invalid key")
    {
        auto callsNotifications = notificationsRecordInterface.GetByKey(NotificationsRecord::Key::NotValidKey);
        REQUIRE_FALSE(callsNotifications.isValidRecord());
        REQUIRE(callsNotifications.ID == DB_ID_NONE);
        REQUIRE(callsNotifications.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(callsNotifications.value == 0);

        auto smsNotifications = notificationsRecordInterface.GetByKey(NotificationsRecord::Key::NumberOfKeys);
        REQUIRE_FALSE(smsNotifications.isValidRecord());
        REQUIRE(smsNotifications.ID == DB_ID_NONE);
        REQUIRE(smsNotifications.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(smsNotifications.value == 0);
    }

    SECTION("Get entries")
    {
        SECTION("Get records using valid offset/limit parameters")
        {
            auto retOffsetLimit = notificationsRecordInterface.GetLimitOffset(0, numberOfNotifcations);
            REQUIRE(retOffsetLimit->size() == numberOfNotifcations);
        }

        SECTION("Get table rows using bigger limit parameters")
        {
            auto retOffsetLimit = notificationsRecordInterface.GetLimitOffset(0, 100);
            REQUIRE(retOffsetLimit->size() == numberOfNotifcations);
        }

        SECTION("Get table rows using invalid offset/limit parameters(should return empty object)")
        {
            auto retOffsetLimit = notificationsRecordInterface.GetLimitOffset(5, 4);
            REQUIRE(retOffsetLimit->size() == 0);
        }

        SECTION("0 - get all")
        {
            auto retOffsetLimit = notificationsRecordInterface.GetLimitOffset(0, 0);
            REQUIRE(retOffsetLimit->size() == numberOfNotifcations);
        }
    }

    SECTION("Entry update value")
    {
        auto entryPre  = notificationsRecordInterface.GetByID(1);
        entryPre.value = entryPre.value + 100;

        REQUIRE(notificationsRecordInterface.Update(entryPre));

        auto entryPost = notificationsRecordInterface.GetByID(1);
        REQUIRE(entryPost.ID == entryPre.ID);
        REQUIRE(entryPost.key == entryPre.key);
        REQUIRE(entryPost.value == entryPre.value);
    }

    SECTION("Entry update key")
    {
        auto entryPre = notificationsRecordInterface.GetByID(1);
        entryPre.key  = NotificationsRecord::Key::Sms;

        REQUIRE_FALSE(notificationsRecordInterface.Update(entryPre));

        auto entryPost = notificationsRecordInterface.GetByID(1);
        REQUIRE(entryPost.ID == entryPre.ID);
        REQUIRE_FALSE(entryPost.key == entryPre.key);
        REQUIRE(entryPost.value == entryPre.value);
    }

    auto getByKey = [&](NotificationsRecord::Key key, uint32_t val) {
        auto query  = std::make_shared<db::query::notifications::Get>(key);
        auto ret    = notificationsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::notifications::GetResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto record = result->getResult();
        REQUIRE(record.isValid());
        REQUIRE(record.key == key);
        REQUIRE(record.value == val);
    };

    auto incrementByKey = [&](NotificationsRecord::Key key) {
        auto query  = std::make_shared<db::query::notifications::Increment>(key);
        auto ret    = notificationsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::notifications::IncrementResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
    };

    auto clearByKey = [&](NotificationsRecord::Key key) {
        auto query  = std::make_shared<db::query::notifications::Clear>(key);
        auto ret    = notificationsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::notifications::ClearResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
    };

    SECTION("Get via query")
    {
        getByKey(NotificationsRecord::Key::Calls, 0);
        getByKey(NotificationsRecord::Key::Sms, 0);
    }

    SECTION("Increment via query")
    {
        auto incrementAndCheckCase = [&](NotificationsRecord::Key key) {
            getByKey(key, 0);
            incrementByKey(key);
            getByKey(key, 1);
            incrementByKey(key);
            incrementByKey(key);
            getByKey(key, 3);
        };

        incrementAndCheckCase(NotificationsRecord::Key::Calls);
        incrementAndCheckCase(NotificationsRecord::Key::Sms);
    }

    SECTION("Clear via query")
    {
        auto clearCase = [&](NotificationsRecord::Key key) {
            getByKey(key, 0);
            for (int i = 0; i < 100; i++)
                incrementByKey(key);
            getByKey(key, 100);
            clearByKey(key);
            getByKey(key, 0);
        };

        clearCase(NotificationsRecord::Key::Calls);
        clearCase(NotificationsRecord::Key::Sms);
    }

    SECTION("Get All via query")
    {
        auto query  = std::make_shared<db::query::notifications::GetAll>();
        auto ret    = notificationsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::notifications::GetAllResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto records = result->getResult();
        REQUIRE(records->size() == numberOfNotifcations);
    }

    Database::deinitialize();
}
