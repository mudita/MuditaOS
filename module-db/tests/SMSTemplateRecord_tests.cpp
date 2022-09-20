// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "Helpers.hpp"
#include "Interface/SMSTemplateRecord.hpp"
#include "module-db/databases/SmsDB.hpp"

#include <algorithm>
#include <cstring>

TEST_CASE("SMS templates Record tests")
{
    db::tests::DatabaseUnderTest<SmsDB> smsDb{"sms.db", db::tests::getPurePhoneScriptsPath()};

    SMSTemplateRecordInterface SMSTemplateRecordInterface(&smsDb.get());
    SMSTemplateRecord testRec;
    testRec.text               = "Test text";
    testRec.lastUsageTimestamp = 100;

    const auto smsRecords = SMSTemplateRecordInterface.GetCount();
    // clear sms table
    for (std::uint32_t id = 1; id <= smsRecords; id++) {
        REQUIRE(SMSTemplateRecordInterface.RemoveByID(id));
    }
    // Add 4 records
    REQUIRE(SMSTemplateRecordInterface.Add(testRec));
    REQUIRE(SMSTemplateRecordInterface.Add(testRec));
    REQUIRE(SMSTemplateRecordInterface.Add(testRec));
    REQUIRE(SMSTemplateRecordInterface.Add(testRec));

    REQUIRE(SMSTemplateRecordInterface.GetCount() == 4);

    SECTION("Get entry by ID")
    {
        auto templ = SMSTemplateRecordInterface.GetByID(4);
        REQUIRE(templ.ID == 4);
        REQUIRE(templ.text == testRec.text);
        REQUIRE(templ.lastUsageTimestamp == testRec.lastUsageTimestamp);
    }

    SECTION("Entry update")
    {
        testRec.ID                 = 4;
        testRec.text               = "New text";
        testRec.lastUsageTimestamp = 200;
        REQUIRE(SMSTemplateRecordInterface.Update(testRec));
        auto templ = SMSTemplateRecordInterface.GetByID(4);
        REQUIRE(templ.ID == 4);
        REQUIRE(templ.text == testRec.text);
        REQUIRE(templ.lastUsageTimestamp == testRec.lastUsageTimestamp);
    }

    SECTION("Get entry - invalid ID")
    {
        auto templ = SMSTemplateRecordInterface.GetByID(100);
        REQUIRE(templ.ID == DB_ID_NONE);
        REQUIRE(templ.text == "");
        REQUIRE(templ.lastUsageTimestamp == 0);
    }

    SECTION("Get table rows")
    {
        // Get table rows using valid offset/limit parameters
        auto retOffsetLimit = SMSTemplateRecordInterface.GetLimitOffset(0, 4);
        REQUIRE(retOffsetLimit->size() == 4);

        // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
        auto retOffsetLimitBigger = SMSTemplateRecordInterface.GetLimitOffset(0, 100);
        REQUIRE(retOffsetLimitBigger->size() == 4);

        // Get table rows using invalid offset/limit parameters(should return empty object)
        auto retOffsetLimitFailed = SMSTemplateRecordInterface.GetLimitOffset(5, 4);
        REQUIRE(retOffsetLimitFailed->size() == 0);
    }

    SECTION("Remove entries")
    {
        REQUIRE(SMSTemplateRecordInterface.RemoveByID(2));

        // Table should have now 3 elements
        REQUIRE(SMSTemplateRecordInterface.GetCount() == 3);

        // Remove non existing element
        REQUIRE(!SMSTemplateRecordInterface.RemoveByID(100));

        // Remove all elements from table
        REQUIRE(SMSTemplateRecordInterface.RemoveByID(1));
        REQUIRE(SMSTemplateRecordInterface.RemoveByID(3));
        REQUIRE(SMSTemplateRecordInterface.RemoveByID(4));

        // Table should be empty now
        REQUIRE(SMSTemplateRecordInterface.GetCount() == 0);
    }
}
