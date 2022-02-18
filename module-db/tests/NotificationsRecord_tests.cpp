// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common.hpp"
#include <catch2/catch.hpp>

#include <Interface/NotificationsRecord.hpp>
#include <Interface/ContactRecord.hpp>
#include <Database/Database.hpp>
#include <Databases/NotificationsDB.hpp>
#include <Databases/ContactsDB.hpp>
#include <queries/notifications/QueryNotificationsGet.hpp>
#include <queries/notifications/QueryNotificationsIncrement.hpp>
#include <queries/notifications/QueryNotificationsDecrement.hpp>
#include <queries/notifications/QueryNotificationsClear.hpp>
#include <queries/notifications/QueryNotificationsGetAll.hpp>

#include <filesystem>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>

TEST_CASE("Notifications Record tests")
{

    SECTION("Default Constructor")
    {
        NotificationsRecord testRec;
        REQUIRE_FALSE(testRec.isValid());
        REQUIRE(testRec.ID == DB_ID_NONE);
        REQUIRE(testRec.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(testRec.value == 0);
        REQUIRE_FALSE(testRec.contactRecord.has_value());
    }

    SECTION("Constructor from NotificationsTableRow")
    {
        NotificationsTableRow tableRow{
            Record(10), .key = static_cast<uint32_t>(NotificationsRecord::Key::Calls), .value = 2};
        NotificationsRecord testRec(tableRow);
        REQUIRE(testRec.isValid());
        REQUIRE(testRec.ID == 10);
        REQUIRE(testRec.key == NotificationsRecord::Key::Calls);
        REQUIRE(testRec.value == 2);
        REQUIRE_FALSE(testRec.contactRecord.has_value());
    }

    Database::initialize();
    const auto notificationsPath = (std::filesystem::path{"sys/user"} / "notifications.db");
    const auto contactsPath      = (std::filesystem::path{"sys/user"} / "contacts.db");
    RemoveDbFiles(notificationsPath.stem());
    RemoveDbFiles(contactsPath.stem());

    NotificationsDB notificationsDb{notificationsPath.c_str()};
    ContactsDB contactsDb{contactsPath.c_str()};
    REQUIRE(notificationsDb.isInitialized());
    REQUIRE(contactsDb.isInitialized());

    const auto notificationsCount = notificationsDb.notifications.count() + 1;
    // clear notifications table
    for (std::size_t id = 1; id <= notificationsCount; id++) {
        REQUIRE(notificationsDb.notifications.removeById(id));
    }

    ContactRecordInterface contactRecordInterface(&contactsDb);
    NotificationsRecordInterface notificationsRecordInterface(&notificationsDb, &contactRecordInterface);
    REQUIRE(contactRecordInterface.GetCount() == 0);
    REQUIRE(notificationsRecordInterface.GetCount() == 0);

    NotificationsTableRow callsRow{
        Record(DB_ID_NONE), .key = static_cast<uint32_t>(NotificationsRecord::Key::Calls), .value = 0};

    REQUIRE(notificationsDb.notifications.add(callsRow));

    NotificationsTableRow smsRow{
        Record(DB_ID_NONE), .key = static_cast<uint32_t>(NotificationsRecord::Key::Sms), .value = 0};

    REQUIRE(notificationsDb.notifications.add(smsRow));
    NotificationsRecord testRec;
    auto numberOfNotifcations = notificationsRecordInterface.GetCount();
    REQUIRE(numberOfNotifcations == 2); // calls and sms notifications

    SECTION("Get entry by ID")
    {
        auto callsNotifications = notificationsRecordInterface.GetByID(1);
        REQUIRE(callsNotifications.isValid());
        REQUIRE(callsNotifications.ID == 1);
        REQUIRE(callsNotifications.key == NotificationsRecord::Key::Calls);
        REQUIRE(callsNotifications.value == 0);
        REQUIRE_FALSE(callsNotifications.contactRecord.has_value());

        auto smsNotifications = notificationsRecordInterface.GetByID(2);
        REQUIRE(smsNotifications.isValid());
        REQUIRE(smsNotifications.ID == 2);
        REQUIRE(smsNotifications.key == NotificationsRecord::Key::Sms);
        REQUIRE(smsNotifications.value == 0);
        REQUIRE_FALSE(smsNotifications.contactRecord.has_value());
    }

    SECTION("Get entry by key")
    {
        auto callsNotifications = notificationsRecordInterface.GetByKey(NotificationsRecord::Key::Calls);
        REQUIRE(callsNotifications.isValid());
        REQUIRE(callsNotifications.ID == 1);
        REQUIRE(callsNotifications.key == NotificationsRecord::Key::Calls);
        REQUIRE(callsNotifications.value == 0);

        auto smsNotifications = notificationsRecordInterface.GetByKey(NotificationsRecord::Key::Sms);
        REQUIRE(smsNotifications.isValid());
        REQUIRE(smsNotifications.ID == 2);
        REQUIRE(smsNotifications.key == NotificationsRecord::Key::Sms);
        REQUIRE(smsNotifications.value == 0);
    }

    SECTION("Get entry - invalid ID")
    {
        auto entry = notificationsRecordInterface.GetByID(100);
        REQUIRE_FALSE(entry.isValid());
        REQUIRE(entry.ID == DB_ID_NONE);
        REQUIRE(entry.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(entry.value == 0);
    }

    SECTION("Get entry by invalid key")
    {
        auto callsNotifications = notificationsRecordInterface.GetByKey(NotificationsRecord::Key::NotValidKey);
        REQUIRE_FALSE(callsNotifications.isValid());
        REQUIRE(callsNotifications.ID == DB_ID_NONE);
        REQUIRE(callsNotifications.key == NotificationsRecord::Key::NotValidKey);
        REQUIRE(callsNotifications.value == 0);

        auto smsNotifications = notificationsRecordInterface.GetByKey(NotificationsRecord::Key::NumberOfKeys);
        REQUIRE_FALSE(smsNotifications.isValid());
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

    auto getByKey = [&](NotificationsRecord::Key key,
                        uint32_t expectedValue,
                        const std::optional<ContactRecord> &expectedContactRecord = std::nullopt) {
        auto query  = std::make_shared<db::query::notifications::Get>(key);
        auto ret    = notificationsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::notifications::GetResult *>(ret.get());
        REQUIRE(result != nullptr);
        auto record = result->getResult();
        REQUIRE(record.isValid());
        REQUIRE(record.key == key);
        REQUIRE(record.value == expectedValue);
        REQUIRE(record.contactRecord.has_value() == expectedContactRecord.has_value());
        if (expectedContactRecord.has_value()) {
            REQUIRE(record.contactRecord.value().ID == expectedContactRecord.value().ID);
        }
    };

    auto incrementByKey = [&](NotificationsRecord::Key key, const std::string &phoneNumber = "+48500500500") {
        utils::PhoneNumber number(phoneNumber);
        auto query  = std::make_shared<db::query::notifications::Increment>(key, number.getView());
        auto ret    = notificationsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::notifications::IncrementResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->getResult());
    };

    auto decrementByKey = [&](NotificationsRecord::Key key, size_t count = 1) {
        auto query  = std::make_shared<db::query::notifications::Decrement>(key, count);
        auto ret    = notificationsRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::notifications::DecrementResult *>(ret.get());
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

    SECTION("Notification DB and Contact DB connection tests")
    {
        const std::string primaryNameTest_1 = "PrimaryTestNameOne";
        const std::string numberUserTest_1  = "600123456";
        ContactRecord testContactRecord_1;
        testContactRecord_1.primaryName = primaryNameTest_1;
        testContactRecord_1.numbers =
            std::vector<ContactRecord::Number>({ContactRecord::Number(utils::PhoneNumber(numberUserTest_1).getView())});

        const std::string primaryNameTest_2 = "PrimaryTestNameTwo";
        const std::string numberUserTest_2  = "600600600";
        ContactRecord testContactRecord_2;
        testContactRecord_2.primaryName = primaryNameTest_2;
        testContactRecord_2.numbers =
            std::vector<ContactRecord::Number>({ContactRecord::Number(utils::PhoneNumber(numberUserTest_2).getView())});

        REQUIRE(contactRecordInterface.Add(testContactRecord_1));
        REQUIRE(contactRecordInterface.Add(testContactRecord_2));

        const auto someUnknownNumber      = "500200300";
        const auto noNotificationExpected = 0;
        const auto contactNotExpected     = std::nullopt;

        SECTION("Tests preconditions")
        {
            getByKey(NotificationsRecord::Key::Calls, noNotificationExpected);
        }

        SECTION("Single call notification from unknown number")
        {
            const auto expectedNotificationValue = 1;
            incrementByKey(NotificationsRecord::Key::Calls, someUnknownNumber);
            getByKey(NotificationsRecord::Key::Calls, expectedNotificationValue, contactNotExpected);
        }

        SECTION("Single call notification from known number")
        {
            const auto expectedNotificationValue = 1;
            incrementByKey(NotificationsRecord::Key::Calls, numberUserTest_1);
            getByKey(NotificationsRecord::Key::Calls, expectedNotificationValue, testContactRecord_1);
            clearByKey(NotificationsRecord::Key::Calls);
            getByKey(NotificationsRecord::Key::Calls, noNotificationExpected, contactNotExpected);
        }

        SECTION("Multiple call notifications from single known number")
        {
            const auto expectedNotificationValue = 3;
            for (int i = 0; i < expectedNotificationValue; i++)
                incrementByKey(NotificationsRecord::Key::Calls, numberUserTest_1);
            getByKey(NotificationsRecord::Key::Calls, expectedNotificationValue, testContactRecord_1);
            clearByKey(NotificationsRecord::Key::Calls);
            getByKey(NotificationsRecord::Key::Calls, noNotificationExpected, contactNotExpected);
        }

        SECTION("Multiple call notifications from multiple known numbers")
        {
            const auto expectedNotificationValue = 2 * 3;
            for (int i = 0; i < expectedNotificationValue / 2; i++) {
                incrementByKey(NotificationsRecord::Key::Calls, numberUserTest_1);
                incrementByKey(NotificationsRecord::Key::Calls, numberUserTest_2);
            }
            getByKey(NotificationsRecord::Key::Calls, expectedNotificationValue, contactNotExpected);
            clearByKey(NotificationsRecord::Key::Calls);
            getByKey(NotificationsRecord::Key::Calls, noNotificationExpected, contactNotExpected);
        }

        SECTION("Multiple call notifications in sequence of known-unknown-known numbers")
        {
            incrementByKey(NotificationsRecord::Key::Calls, numberUserTest_1);
            getByKey(NotificationsRecord::Key::Calls, 1, testContactRecord_1);

            incrementByKey(NotificationsRecord::Key::Calls, someUnknownNumber);
            getByKey(NotificationsRecord::Key::Calls, 2, contactNotExpected);

            incrementByKey(NotificationsRecord::Key::Calls, numberUserTest_1);
            getByKey(NotificationsRecord::Key::Calls, 3, contactNotExpected);
        }

        SECTION("Multiple call notifications in sequence of unknown-known-unknown numbers")
        {
            incrementByKey(NotificationsRecord::Key::Calls, someUnknownNumber);
            getByKey(NotificationsRecord::Key::Calls, 1, contactNotExpected);

            incrementByKey(NotificationsRecord::Key::Calls, numberUserTest_1);
            getByKey(NotificationsRecord::Key::Calls, 2, contactNotExpected);

            incrementByKey(NotificationsRecord::Key::Calls, someUnknownNumber);
            getByKey(NotificationsRecord::Key::Calls, 3, contactNotExpected);
        }

        SECTION("Clear SMS notifications from multiple known numbers")
        {
            const auto expectedNotificationPerContact = 3;
            const auto expectedNotificationValue      = 2 * expectedNotificationPerContact;
            for (int i = 0; i < expectedNotificationValue / 2; i++) {
                incrementByKey(NotificationsRecord::Key::Sms, numberUserTest_1);
                incrementByKey(NotificationsRecord::Key::Sms, numberUserTest_2);
            }
            getByKey(NotificationsRecord::Key::Sms, expectedNotificationValue, contactNotExpected);

            decrementByKey(NotificationsRecord::Key::Sms, expectedNotificationPerContact);
            getByKey(NotificationsRecord::Key::Sms, expectedNotificationPerContact, contactNotExpected);

            decrementByKey(NotificationsRecord::Key::Sms, expectedNotificationPerContact);
            getByKey(NotificationsRecord::Key::Sms, noNotificationExpected, contactNotExpected);
        }

        SECTION("Clear SMS notifications overflowing actual count")
        {
            const auto expectedNotificationPerContact = 3;
            const auto expectedNotificationValue      = 2 * expectedNotificationPerContact;
            for (int i = 0; i < expectedNotificationValue / 2; i++) {
                incrementByKey(NotificationsRecord::Key::Sms, numberUserTest_1);
                incrementByKey(NotificationsRecord::Key::Sms, numberUserTest_2);
            }
            getByKey(NotificationsRecord::Key::Sms, expectedNotificationValue, contactNotExpected);

            decrementByKey(NotificationsRecord::Key::Sms, expectedNotificationPerContact + 1);
            getByKey(NotificationsRecord::Key::Sms, 2, contactNotExpected);

            decrementByKey(NotificationsRecord::Key::Sms, expectedNotificationPerContact);
            getByKey(NotificationsRecord::Key::Sms, noNotificationExpected, contactNotExpected);
        }
    }

    Database::deinitialize();
}
