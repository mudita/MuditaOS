
#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/ContactsDB.hpp"
#include "Databases/SmsDB.hpp"
#include "Interface/ContactRecord.hpp"
#include "Interface/SMSRecord.hpp"
#include "Interface/ThreadRecord.hpp"
#include "queries/sms/QuerySmsThreadMarkAsRead.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include "vfs.hpp"

#include <algorithm>

#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Thread Record tests")
{
    Database::initialize();

    vfs.remove(ContactsDB::GetDBName());
    vfs.remove(SmsDB::GetDBName());

    auto smsDB = std::make_unique<SmsDB>();
    REQUIRE(smsDB->isInitialized());
    auto contactsDB = std::make_unique<ContactsDB>();
    REQUIRE(contactsDB->isInitialized());

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
    SECTION("Get all available records with query")
    {
        auto query  = std::make_shared<db::query::SMSThreadsGet>(0, 100);
        auto ret    = threadRecordInterface1.runQuery(query);
        auto result = dynamic_cast<db::query::SMSThreadsGetResults *>(ret.get());
        REQUIRE(result != nullptr);
        auto results = result->getResults();
        REQUIRE(results.size() == 2);

        // Check if fetched records contain valid data
        for (const auto &w : results) {
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
            auto query =
                std::make_shared<db::query::smsthread::MarkAsRead>(3, db::query::smsthread::MarkAsRead::Read::True);
            auto ret    = threadRecordInterface1.runQuery(query);
            auto result = dynamic_cast<db::query::smsthread::MarkAsReadResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
            rec = threadRecordInterface1.GetByID(3);
            REQUIRE_FALSE(rec.isUnread());
        }

        {
            auto query =
                std::make_shared<db::query::smsthread::MarkAsRead>(3, db::query::smsthread::MarkAsRead::Read::False);
            auto ret    = threadRecordInterface1.runQuery(query);
            auto result = dynamic_cast<db::query::smsthread::MarkAsReadResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
            rec = threadRecordInterface1.GetByID(3);
            REQUIRE(rec.isUnread());
        }
    }

    SECTION("SMS search")
    {
        SMSRecordInterface smsRecInterface(smsDB.get(), contactsDB.get());
        SMSRecord recordIN;
        recordIN.date      = 123456789;
        recordIN.dateSent  = 987654321;
        recordIN.errorCode = 0;
        recordIN.number    = utils::PhoneNumber("+48600123456", utils::country::Id::UNKNOWN).getView();
        recordIN.body      = "Ala";
        recordIN.type      = SMSType ::DRAFT;

        REQUIRE(smsRecInterface.Add(recordIN));
        recordIN.body = "Ola";
        REQUIRE(smsRecInterface.Add(recordIN));

        {
            auto query  = std::make_shared<db::query::SMSSearch>("A", 0, 10);
            auto ret    = threadRecordInterface1.runQuery(query);
            auto result = dynamic_cast<db::query::SMSSearchResult *>(ret.get());
            REQUIRE(result != nullptr);
            auto results = result->getResults();
            REQUIRE(results.size() == 2);
        }

        {
            auto query  = std::make_shared<db::query::SMSSearch>("O", 0, 10);
            auto ret    = threadRecordInterface1.runQuery(query);
            auto result = dynamic_cast<db::query::SMSSearchResult *>(ret.get());
            REQUIRE(result != nullptr);
            auto results = result->getResults();
            REQUIRE(results.size() == 1);
        }
    }

    Database::deinitialize();
}
