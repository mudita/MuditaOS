#include "catch.hpp"

#include "Database/Database.hpp"
#include "Databases/SmsDB.hpp"

#include "Tables/SMSTemplateTable.hpp"

#include <vfs.hpp>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>

TEST_CASE("SMS Templates Table tests")
{
    Database::Initialize();

    vfs.remove(SmsDB::GetDBName());

    SmsDB smsDb;
    REQUIRE(smsDb.IsInitialized());

    auto &templatesTbl = smsDb.templates;

    SMSTemplateTableRow testRow = {{.ID = 0}, .text = "Test text", .lastUsageTimestamp = 100};

    // Add 4 elements into table
    REQUIRE(templatesTbl.Add(testRow));
    REQUIRE(templatesTbl.Add(testRow));
    REQUIRE(templatesTbl.Add(testRow));
    REQUIRE(templatesTbl.Add(testRow));

    REQUIRE(templatesTbl.GetCount() == 4);

    SECTION("Get entry by ID")
    {
        auto templ = templatesTbl.GetByID(4);
        REQUIRE(templ.ID == 4);
        REQUIRE(templ.text == testRow.text);
        REQUIRE(templ.lastUsageTimestamp == testRow.lastUsageTimestamp);
    }

    SECTION("Entry update")
    {
        testRow.ID                 = 4;
        testRow.text               = "New text";
        testRow.lastUsageTimestamp = 200;
        REQUIRE(templatesTbl.Update(testRow));
        auto templ = templatesTbl.GetByID(4);
        REQUIRE(templ.ID == 4);
        REQUIRE(templ.text == testRow.text);
        REQUIRE(templ.lastUsageTimestamp == testRow.lastUsageTimestamp);
    }

    SECTION("Get entry - invalid ID")
    {
        auto templ = templatesTbl.GetByID(100);
        REQUIRE(templ.ID == DB_ID_NONE);
        REQUIRE(templ.text == "");
        REQUIRE(templ.lastUsageTimestamp == 0);
    }

    SECTION("Get table rows")
    {
        // Get table rows using valid offset/limit parameters
        auto retOffsetLimit = templatesTbl.GetLimitOffset(0, 4);
        REQUIRE(retOffsetLimit.size() == 4);

        // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
        auto retOffsetLimitBigger = templatesTbl.GetLimitOffset(0, 100);
        REQUIRE(retOffsetLimitBigger.size() == 4);

        // Get table rows using invalid offset/limit parameters(should return empty object)
        auto retOffsetLimitFailed = templatesTbl.GetLimitOffset(5, 4);
        REQUIRE(retOffsetLimitFailed.size() == 0);
    }

    SECTION("Remove entries")
    {
        REQUIRE(templatesTbl.RemoveByID(2));

        // Table should have now 3 elements
        REQUIRE(templatesTbl.GetCount() == 3);

        // Remove non existing element
        REQUIRE(templatesTbl.RemoveByID(100));

        // Remove all elements from table
        REQUIRE(templatesTbl.RemoveByID(1));
        REQUIRE(templatesTbl.RemoveByID(3));
        REQUIRE(templatesTbl.RemoveByID(4));

        // Table should be empty now
        REQUIRE(templatesTbl.GetCount() == 0);
    }

    Database::Deinitialize();
}
