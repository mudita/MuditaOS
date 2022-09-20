// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Helpers.hpp"
#include "Interface/CalllogRecord.hpp"
#include "Database/Database.hpp"
#include "module-db/databases/CalllogDB.hpp"

#include <cstring>
#include <filesystem>
#include <algorithm>

TEST_CASE("Calllog Record tests")
{
    db::tests::DatabaseUnderTest<CalllogDB> calllogDb{"calllog.db", db::tests::getPurePhoneScriptsPath()};
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    SECTION("Default Constructor")
    {
        CalllogRecord testRec;
        REQUIRE(testRec.ID == DB_ID_NONE);
        REQUIRE(testRec.presentation == PresentationType::PR_UNKNOWN);
        REQUIRE(testRec.date == 0);
        REQUIRE(testRec.duration == 0);
        REQUIRE(testRec.type == CallType::CT_NONE);
        REQUIRE(!testRec.phoneNumber.isValid());
        REQUIRE(testRec.isRead == true);
    }

    CalllogRecordInterface calllogRecordInterface(&calllogDb.get(), &contactsDb.get());
    CalllogRecord testRec;
    testRec.presentation = PresentationType::PR_ALLOWED;
    testRec.date         = 100;
    testRec.duration     = 100;
    testRec.type         = CallType::CT_INCOMING;
    testRec.phoneNumber  = utils::PhoneNumber("600123456").getView();
    testRec.isRead       = false;

    const auto callsCount = calllogRecordInterface.GetCount() + 1;
    // clear calllog table
    for (std::size_t id = 1; id < callsCount; id++) {
        REQUIRE(calllogRecordInterface.RemoveByID(id));
    }
    // Add 4 records
    REQUIRE(calllogRecordInterface.Add(testRec));
    REQUIRE(calllogRecordInterface.Add(testRec));
    REQUIRE(calllogRecordInterface.Add(testRec));
    REQUIRE(calllogRecordInterface.Add(testRec));

    REQUIRE(calllogRecordInterface.GetCount() == 4);

    SECTION("Get entry by ID")
    {
        auto call = calllogRecordInterface.GetByID(1);
        REQUIRE(call.ID == 1);
        REQUIRE(call.presentation == testRec.presentation);
        REQUIRE(call.date == testRec.date);
        REQUIRE(call.duration == testRec.duration);
        REQUIRE(call.type == testRec.type);
        REQUIRE(call.phoneNumber == testRec.phoneNumber);
        REQUIRE(call.isRead == testRec.isRead);
    }

    SECTION("Entry update")
    {
        auto callPre         = calllogRecordInterface.GetByID(1);
        callPre.presentation = PresentationType::PR_PAYPHONE;
        callPre.date         = callPre.date + 100;
        callPre.duration     = callPre.duration + 100;
        callPre.type         = CallType::CT_MISSED;
        callPre.isRead       = false;

        REQUIRE(calllogRecordInterface.Update(callPre));

        auto callPost = calllogRecordInterface.GetByID(1);
        REQUIRE(callPost.presentation == callPre.presentation);
        REQUIRE(callPost.date == callPre.date);
        REQUIRE(callPost.duration == callPre.duration);
        REQUIRE(callPost.type == callPre.type);
        REQUIRE(callPost.isRead == callPre.isRead);
        REQUIRE(callPost.phoneNumber == callPre.phoneNumber);
    }

    SECTION("Get entry - invalid ID")
    {
        auto call = calllogRecordInterface.GetByID(100);
        REQUIRE(call.ID == DB_ID_NONE);
        REQUIRE(call.presentation == PresentationType::PR_UNKNOWN);
        REQUIRE(call.date == 0);
        REQUIRE(call.duration == 0);
        REQUIRE(call.type == CallType::CT_NONE);
        REQUIRE(!call.phoneNumber.isValid());
        REQUIRE(call.isRead == true);
    }

    SECTION("Get entries")
    {
        SECTION("Get table rows using valid offset/limit parameters")
        {
            auto retOffsetLimit = calllogRecordInterface.GetLimitOffset(0, 4);
            REQUIRE(retOffsetLimit->size() == 4);
        }

        SECTION("Get table rows using invalid limit parameters(should return 4 elements instead of 100)")
        {
            auto retOffsetLimit = calllogRecordInterface.GetLimitOffset(0, 100);
            REQUIRE(retOffsetLimit->size() == 4);
        }

        SECTION("Get table rows using invalid offset/limit parameters(should return empty object)")
        {
            auto retOffsetLimit = calllogRecordInterface.GetLimitOffset(5, 4);
            REQUIRE(retOffsetLimit->size() == 0);
        }
    }

    SECTION("Remove entries")
    {
        REQUIRE(calllogRecordInterface.RemoveByID(2));

        // Table should have now 3 elements
        REQUIRE(calllogRecordInterface.GetCount() == 3);

        // Remove non existing element
        REQUIRE(!calllogRecordInterface.RemoveByID(100));

        // Remove all elements from table
        REQUIRE(calllogRecordInterface.RemoveByID(1));
        REQUIRE(calllogRecordInterface.RemoveByID(3));
        REQUIRE(calllogRecordInterface.RemoveByID(4));

        // Table should be empty now
        REQUIRE(calllogRecordInterface.GetCount() == 0);
    }

    SECTION("Get Count")
    {
        REQUIRE(calllogRecordInterface.GetCount() == 4);
        REQUIRE(calllogRecordInterface.GetCount(EntryState::ALL) == 4);
        REQUIRE(calllogRecordInterface.GetCount(EntryState::READ) == 0);
        REQUIRE(calllogRecordInterface.GetCount(EntryState::UNREAD) == 4);
    }

    SECTION("Set All Read")
    {
        REQUIRE(calllogRecordInterface.GetCount(EntryState::UNREAD) == 4);
        REQUIRE(calllogRecordInterface.GetCount(EntryState::READ) == 0);
        REQUIRE(calllogRecordInterface.SetAllRead());
        REQUIRE(calllogRecordInterface.GetCount(EntryState::UNREAD) == 0);
        REQUIRE(calllogRecordInterface.GetCount(EntryState::READ) == 4);
    }

    SECTION("Set All Read via query")
    {
        REQUIRE(calllogRecordInterface.GetCount(EntryState::UNREAD) == 4);
        REQUIRE(calllogRecordInterface.GetCount(EntryState::READ) == 0);

        auto query  = std::make_shared<db::query::calllog::SetAllRead>();
        auto ret    = calllogRecordInterface.runQuery(query);
        auto result = dynamic_cast<db::query::calllog::SetAllReadResult *>(ret.get());
        REQUIRE(result != nullptr);
        REQUIRE(result->ret == true);

        REQUIRE(calllogRecordInterface.GetCount(EntryState::UNREAD) == 0);
        REQUIRE(calllogRecordInterface.GetCount(EntryState::READ) == 4);
    }
}
