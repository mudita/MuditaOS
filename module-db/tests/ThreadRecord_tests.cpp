// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Helpers.hpp"

#include <Database/Database.hpp>
#include "module-db/databases/ContactsDB.hpp"
#include "module-db/databases/SmsDB.hpp"
#include <Interface/ContactRecord.hpp>
#include <Interface/SMSRecord.hpp>
#include <Interface/ThreadRecord.hpp>
#include <queries/messages/threads/QueryThreadMarkAsRead.hpp>
#include <queries/messages/threads/QueryThreadsSearchForList.hpp>
#include <queries/messages/threads/QueryThreadGetByID.hpp>
#include <queries/messages/threads/QueryThreadGetByNumber.hpp>
#include <queries/messages/threads/QueryThreadRemove.hpp>
#include <queries/messages/threads/QueryThreadsGet.hpp>
#include <queries/messages/sms/QuerySMSGetLastByThreadID.hpp>

#include <catch2/catch.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>

TEST_CASE("Thread Record tests")
{
    db::tests::DatabaseUnderTest<SmsDB> smsDb{"sms.db", db::tests::getPurePhoneScriptsPath()};
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    const uint32_t dateTest  = 123456789;
    const char *snippetTest  = "Test snippet";
    const char *snippetTest2 = "Test snippet2";
    const SMSType typeTest   = SMSType::UNKNOWN;

    ThreadRecordInterface threadRecordInterface1(&smsDb.get(), &contactsDb.get());

    ThreadRecord recordIN;
    recordIN.date    = dateTest;
    recordIN.snippet = snippetTest;
    recordIN.type    = typeTest;

    const auto threadRecords = threadRecordInterface1.GetCount() + 1;
    // clear all records
    for (std::size_t id = 1; id < threadRecords; id++) {
        REQUIRE(threadRecordInterface1.RemoveByID(id));
    }

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
        }
    }

    SECTION("Get all available records with query")
    {
        auto query  = std::make_shared<db::query::ThreadsGet>(0, 100);
        auto ret    = threadRecordInterface1.runQuery(query);
        auto result = dynamic_cast<db::query::ThreadsGetResults *>(ret.get());
        REQUIRE(result != nullptr);
        auto results = result->getResults();
        REQUIRE(results.size() == 2);

        // Check if fetched records contain valid data
        for (const auto &w : results) {
            REQUIRE(w.date == dateTest);
            REQUIRE(w.snippet == snippetTest);
            REQUIRE(w.type == typeTest);
        }
    }

    SECTION("Get record by id with query")
    {
        auto query  = std::make_shared<db::query::ThreadGetByID>(1);
        auto ret    = threadRecordInterface1.runQuery(query);
        auto result = dynamic_cast<db::query::ThreadGetByIDResult *>(ret.get());

        REQUIRE(result->getRecord().has_value());
    }

    SECTION("Remove records one by one")
    {
        REQUIRE(threadRecordInterface1.RemoveByID(1));
        REQUIRE(threadRecordInterface1.RemoveByID(2));

        // SMS database should not contain any records
        REQUIRE(threadRecordInterface1.GetCount() == 0);
    }

    SECTION("Remove records with query")
    {
        auto query1  = std::make_shared<db::query::ThreadRemove>(1);
        auto ret1    = threadRecordInterface1.runQuery(query1);
        auto result1 = dynamic_cast<db::query::ThreadRemoveResult *>(ret1.get());
        REQUIRE(result1->success());

        auto query2  = std::make_shared<db::query::ThreadRemove>(2);
        auto ret2    = threadRecordInterface1.runQuery(query2);
        auto result2 = dynamic_cast<db::query::ThreadRemoveResult *>(ret2.get());
        REQUIRE(result2->success());

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
            auto query  = std::make_shared<db::query::MarkAsRead>(3, db::query::MarkAsRead::Read::True);
            auto ret    = threadRecordInterface1.runQuery(query);
            auto result = dynamic_cast<db::query::MarkAsReadResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
            rec = threadRecordInterface1.GetByID(3);
            REQUIRE_FALSE(rec.isUnread());
        }

        {
            auto query  = std::make_shared<db::query::MarkAsRead>(3, db::query::MarkAsRead::Read::False);
            auto ret    = threadRecordInterface1.runQuery(query);
            auto result = dynamic_cast<db::query::MarkAsReadResult *>(ret.get());
            REQUIRE(result != nullptr);
            REQUIRE(result->getResult());
            rec = threadRecordInterface1.GetByID(3);
            REQUIRE(rec.isUnread());
        }
    }

    SECTION("SMS search")
    {
        const utils::PhoneNumber phoneNumber("+48600123456", utils::country::Id::UNKNOWN);
        const std::string lastSmsBody = "Ola";

        SMSRecordInterface smsRecInterface(&smsDb.get(), &contactsDb.get());
        SMSRecord recordIN;
        recordIN.date      = 123456789;
        recordIN.errorCode = 0;
        recordIN.number    = phoneNumber.getView();
        recordIN.body      = "Ala";
        recordIN.type      = SMSType::DRAFT;

        REQUIRE(smsRecInterface.Add(recordIN));

        recordIN.body = lastSmsBody;
        REQUIRE(smsRecInterface.Add(recordIN));

        {
            auto query  = std::make_shared<db::query::ThreadsSearchForList>("A", 0, 10);
            auto ret    = threadRecordInterface1.runQuery(query);
            auto result = dynamic_cast<db::query::ThreadsSearchResultForList *>(ret.get());
            REQUIRE(result != nullptr);
            auto results = result->getResults();
            REQUIRE(results.size() == 2);
        }

        {
            auto query  = std::make_shared<db::query::ThreadsSearchForList>("O", 0, 10);
            auto ret    = threadRecordInterface1.runQuery(query);
            auto result = dynamic_cast<db::query::ThreadsSearchResultForList *>(ret.get());
            REQUIRE(result != nullptr);
            auto results = result->getResults();
            REQUIRE(results.size() == 1);
        }

        SECTION("Get last SMS by thread id")
        {
            auto getThreadQuery  = std::make_shared<db::query::ThreadGetByNumber>(phoneNumber.getView());
            auto getThreadRet    = threadRecordInterface1.runQuery(getThreadQuery);
            auto getThreatResult = dynamic_cast<db::query::ThreadGetByNumberResult *>(getThreadRet.get());
            REQUIRE(getThreatResult != nullptr);
            auto threadRec = getThreatResult->getThread();
            REQUIRE(threadRec.isValid());

            auto getLastQuery  = std::make_shared<db::query::SMSGetLastByThreadID>(threadRec.ID);
            auto getLastRet    = smsRecInterface.runQuery(getLastQuery);
            auto getLastResult = dynamic_cast<db::query::SMSGetLastByThreadIDResult *>(getLastRet.get());
            REQUIRE(getLastResult != nullptr);
            auto smsRec = getLastResult->record;
            REQUIRE(smsRec.has_value());
            REQUIRE(smsRec->body == lastSmsBody);
        }
    }

    SECTION("Thread with end of line in the body")
    {

        const utils::PhoneNumber phoneNumber("+48600123456", utils::country::Id::UNKNOWN);

        SMSRecordInterface smsRecInterface(&smsDb.get(), &contactsDb.get());
        SMSRecord recordIN;
        recordIN.date      = 123456789;
        recordIN.errorCode = 0;
        recordIN.number    = phoneNumber.getView();
        recordIN.type      = SMSType::DRAFT;

        UTF8 snippetIncluded = "Good 😁IS GOOD";
        UTF8 snippetExcluded = "\nthis part should not be included in snippet";
        UTF8 smsBody         = snippetIncluded + snippetExcluded;

        recordIN.body = smsBody;
        REQUIRE(smsRecInterface.Add(recordIN));

        auto getThreadQuery  = std::make_shared<db::query::ThreadGetByNumber>(phoneNumber.getView());
        auto getThreadRet    = threadRecordInterface1.runQuery(getThreadQuery);
        auto getThreatResult = dynamic_cast<db::query::ThreadGetByNumberResult *>(getThreadRet.get());
        REQUIRE(getThreatResult != nullptr);
        auto threadRec = getThreatResult->getThread();
        REQUIRE(threadRec.isValid());

        SECTION("Snippet should not contain characters after EoL")
        {
            REQUIRE(snippetIncluded == threadRec.snippet);
        }

        SECTION("Body content should not be affected by EoL")
        {
            auto getLastQuery  = std::make_shared<db::query::SMSGetLastByThreadID>(threadRec.ID);
            auto getLastRet    = smsRecInterface.runQuery(getLastQuery);
            auto getLastResult = dynamic_cast<db::query::SMSGetLastByThreadIDResult *>(getLastRet.get());
            REQUIRE(getLastResult != nullptr);
            auto smsRec = getLastResult->record;
            REQUIRE(smsRec.has_value());
            REQUIRE(smsRec->body == smsBody);
        }
    }
}
