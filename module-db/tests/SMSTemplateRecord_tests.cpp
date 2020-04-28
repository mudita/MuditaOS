#include "catch.hpp"

#include "Interface/SMSTemplateRecord.hpp"
#include "Database/Database.hpp"
#include "Databases/SmsDB.hpp"

#include <vfs.hpp>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>

TEST_CASE("SMS templates Record tests")
{
    Database::Initialize();

    vfs.remove(SmsDB::GetDBName());

    auto smsDB = std::make_unique<SmsDB>();
    REQUIRE(smsDB->IsInitialized());

    SMSTemplatesRecordInterface smsTemplatesRecordInterface(smsDB.get());
    SMSTemplatesRecord testRec;
    testRec.text               = "Test text";
    testRec.lastUsageTimestamp = 100;

    // Add 4 records
    REQUIRE(smsTemplatesRecordInterface.Add(testRec));
    REQUIRE(smsTemplatesRecordInterface.Add(testRec));
    REQUIRE(smsTemplatesRecordInterface.Add(testRec));
    REQUIRE(smsTemplatesRecordInterface.Add(testRec));

    REQUIRE(smsTemplatesRecordInterface.GetCount() == 4);

    SECTION("Get entry by ID")
    {
        auto templ = smsTemplatesRecordInterface.GetByID(4);
        REQUIRE(templ.ID == 4);
        REQUIRE(templ.text == testRec.text);
        REQUIRE(templ.lastUsageTimestamp == testRec.lastUsageTimestamp);
    }

    SECTION("Entry update")
    {
        testRec.ID                 = 4;
        testRec.text               = "New text";
        testRec.lastUsageTimestamp = 200;
        REQUIRE(smsTemplatesRecordInterface.Update(testRec));
        auto templ = smsTemplatesRecordInterface.GetByID(4);
        REQUIRE(templ.ID == 4);
        REQUIRE(templ.text == testRec.text);
        REQUIRE(templ.lastUsageTimestamp == testRec.lastUsageTimestamp);
    }

    SECTION("Get entry - invalid ID")
    {
        auto templ = smsTemplatesRecordInterface.GetByID(100);
        REQUIRE(templ.ID == DB_ID_NONE);
        REQUIRE(templ.text == "");
        REQUIRE(templ.lastUsageTimestamp == 0);
    }

    SECTION("Get table rows")
    {
        // Get table rows using valid offset/limit parameters
        auto retOffsetLimit = smsTemplatesRecordInterface.GetLimitOffset(0, 4);
        REQUIRE(retOffsetLimit->size() == 4);

        // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
        auto retOffsetLimitBigger = smsTemplatesRecordInterface.GetLimitOffset(0, 100);
        REQUIRE(retOffsetLimitBigger->size() == 4);

        // Get table rows using invalid offset/limit parameters(should return empty object)
        auto retOffsetLimitFailed = smsTemplatesRecordInterface.GetLimitOffset(5, 4);
        REQUIRE(retOffsetLimitFailed->size() == 0);
    }

    SECTION("Remove entries")
    {
        REQUIRE(smsTemplatesRecordInterface.RemoveByID(2));

        // Table should have now 3 elements
        REQUIRE(smsTemplatesRecordInterface.GetCount() == 3);

        // Remove non existing element
        REQUIRE(!smsTemplatesRecordInterface.RemoveByID(100));

        // Remove all elements from table
        REQUIRE(smsTemplatesRecordInterface.RemoveByID(1));
        REQUIRE(smsTemplatesRecordInterface.RemoveByID(3));
        REQUIRE(smsTemplatesRecordInterface.RemoveByID(4));

        // Table should be empty now
        REQUIRE(smsTemplatesRecordInterface.GetCount() == 0);
    }

    Database::Deinitialize();
}
