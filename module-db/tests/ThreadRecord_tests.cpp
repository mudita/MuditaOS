
/*
 * @file ThreadRecord_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 11.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/ContactsDB.hpp"
#include "Databases/SmsDB.hpp"
#include "Interface/ContactRecord.hpp"
#include "Interface/SMSRecord.hpp"
#include "Interface/ThreadRecord.hpp"
#include "queries/sms/QuerySmsThreadMarkAsRead.hpp"
#include "vfs.hpp"

#include <algorithm>

#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Thread Record tests")
{
    Database::Initialize();

    vfs.remove(ContactsDB::GetDBName());
    vfs.remove(SmsDB::GetDBName());

    auto smsDB = std::make_unique<SmsDB>();
    REQUIRE(smsDB->IsInitialized());
    auto contactsDB = std::make_unique<ContactsDB>();
    REQUIRE(contactsDB->IsInitialized());

    const uint32_t dateTest      = 123456789;
    const char *snippetTest      = "Test snippet";
    const char *snippetTest2     = "Test snippet2";
    const SMSType typeTest       = SMSType ::UNKNOWN;
    const uint32_t contactIDTest = 100;

    ThreadRecordInterface threadRecordInterface1(smsDB.get(), contactsDB.get());

    ThreadRecord recordIN;
    recordIN.date      = dateTest;
    recordIN.snippet   = snippetTest;
    recordIN.type      = typeTest;
    recordIN.contactID = contactIDTest;

    // Add 2 records
    REQUIRE(threadRecordInterface1.Add(recordIN));
    REQUIRE(threadRecordInterface1.Add(recordIN));

    SECTION("isUnread")
    {
        REQUIRE_FALSE(recordIN.isUnread());
        recordIN.unreadMsgCount = 10;
        REQUIRE(recordIN.isUnread());
    }

    SECTION("Get Count")
    {
        recordIN.unreadMsgCount = 10;
        REQUIRE(threadRecordInterface1.Add(recordIN));
        REQUIRE(threadRecordInterface1.GetCount() == 3);
        REQUIRE(threadRecordInterface1.GetCount(EntryState::ALL) == 3);
        REQUIRE(threadRecordInterface1.GetCount(EntryState::READ) == 2);
        REQUIRE(threadRecordInterface1.GetCount(EntryState::UNREAD) == 1);
    }

    SECTION("Get all available records")
    {
        auto records = threadRecordInterface1.GetLimitOffset(0, 100);
        REQUIRE((*records).size() == 2);

        // Check if fetched records contain valid data
        for (const auto &w : *records) {
            REQUIRE(w.date == dateTest);
            REQUIRE(w.snippet == snippetTest);
            REQUIRE(w.type == typeTest);
            REQUIRE(w.contactID == contactIDTest);
        }
    }

    SECTION("Get all available records by specified thread ID and check for invalid data")
    {
        auto records = threadRecordInterface1.GetLimitOffsetByField(
            0, 100, ThreadRecordField ::ContactID, std::to_string(contactIDTest).c_str());
        REQUIRE((*records).size() == 2);
        for (const auto &w : *records) {
            REQUIRE(w.date == dateTest);
            REQUIRE(w.snippet == snippetTest);
            REQUIRE(w.type == typeTest);
            REQUIRE(w.contactID == contactIDTest);
        }
    }

    SECTION("Remove records one by one")
    {
        REQUIRE(threadRecordInterface1.RemoveByID(1));
        REQUIRE(threadRecordInterface1.RemoveByID(2));

        // SMS database should not contain any records
        REQUIRE(threadRecordInterface1.GetCount() == 0);
    }

    SECTION("Test updating record")
    {
        // REQUIRE(threadRecordInterface1.Add(recordIN));
        recordIN.ID      = 1;
        recordIN.snippet = snippetTest2;
        REQUIRE(threadRecordInterface1.Update(recordIN));

        auto record = threadRecordInterface1.GetByID(1);
        REQUIRE(record.isValid());
        REQUIRE(record.snippet == snippetTest2);
    }

    SECTION("Mark as read / unread")
    {
        recordIN.unreadMsgCount = 10;
        REQUIRE(threadRecordInterface1.Add(recordIN));
        auto rec = threadRecordInterface1.GetByID(3);
        REQUIRE(rec.isUnread());

        {
            db::query::smsthread::MarkAsRead query{3, db::query::smsthread::MarkAsRead::Read::True};
            auto ret    = threadRecordInterface1.runQuery(&query);
            auto result = dynamic_cast<db::query::smsthread::MarkAsReadResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
            rec = threadRecordInterface1.GetByID(3);
            REQUIRE_FALSE(rec.isUnread());
        }

        {
            db::query::smsthread::MarkAsRead query{3, db::query::smsthread::MarkAsRead::Read::False};
            auto ret    = threadRecordInterface1.runQuery(&query);
            auto result = dynamic_cast<db::query::smsthread::MarkAsReadResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
            rec = threadRecordInterface1.GetByID(3);
            REQUIRE(rec.isUnread());
        }
    }

    Database::Deinitialize();
}
