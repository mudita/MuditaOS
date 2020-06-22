#include "catch.hpp"

#include "Database/Database.hpp"
#include "Databases/CalllogDB.hpp"

#include "Tables/CalllogTable.hpp"

#include <vfs.hpp>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>

TEST_CASE("Calllog Table tests")
{
    Database::Initialize();

    vfs.remove(CalllogDB::GetDBName());

    CalllogDB calllogDb;
    REQUIRE(calllogDb.IsInitialized());

    auto &callsTbl = calllogDb.calls;

    SECTION("Default Constructor")
    {
        CalllogTableRow testRow;
        REQUIRE(testRow.ID == DB_ID_NONE);
        REQUIRE(testRow.number == "");
        REQUIRE(testRow.e164number == "");
        REQUIRE(testRow.presentation == PresentationType::PR_UNKNOWN);
        REQUIRE(testRow.date == 0);
        REQUIRE(testRow.duration == 0);
        REQUIRE(testRow.type == CallType::CT_NONE);
        REQUIRE(testRow.name == "");
        REQUIRE(testRow.contactId == "");
        REQUIRE(testRow.isRead == true);
    }

    CalllogTableRow testRow = {{.ID = 0},
                               .number       = "600123456",
                               .e164number   = "+48600226908",
                               .presentation = PresentationType::PR_ALLOWED,
                               .date         = 0,
                               .duration     = 100,
                               .type         = CallType::CT_INCOMING,
                               .name         = "Test name",
                               .contactId    = "1",
                               .isRead       = false};

    // Add 4 elements into table
    REQUIRE(callsTbl.Add(testRow));
    REQUIRE(callsTbl.Add(testRow));
    REQUIRE(callsTbl.Add(testRow));
    REQUIRE(callsTbl.Add(testRow));

    REQUIRE(callsTbl.GetCount() == 4);

    SECTION("Get entry by ID")
    {
        auto callEntry = callsTbl.GetByID(4);
        REQUIRE(callEntry.ID == 4);
        REQUIRE(callEntry.number == testRow.number);
        REQUIRE(callEntry.e164number == testRow.e164number);
        REQUIRE(callEntry.presentation == testRow.presentation);
        REQUIRE(callEntry.date == testRow.date);
        REQUIRE(callEntry.duration == testRow.duration);
        REQUIRE(callEntry.type == testRow.type);
        REQUIRE(callEntry.name == testRow.name);
        REQUIRE(callEntry.contactId == testRow.contactId);
        REQUIRE(callEntry.isRead == testRow.isRead);
    }

    SECTION("Entry update")
    {
        testRow.ID     = 3;
        testRow.number = "500888999";
        REQUIRE(callsTbl.Update(testRow));
        auto callEntry = callsTbl.GetByID(3);
        REQUIRE(callEntry.ID == 3);
        REQUIRE(callEntry.number == testRow.number);
        REQUIRE(callEntry.e164number == testRow.e164number);
        REQUIRE(callEntry.presentation == testRow.presentation);
        REQUIRE(callEntry.date == testRow.date);
        REQUIRE(callEntry.duration == testRow.duration);
        REQUIRE(callEntry.type == testRow.type);
        REQUIRE(callEntry.name == testRow.name);
        REQUIRE(callEntry.contactId == testRow.contactId);
        REQUIRE(callEntry.isRead == testRow.isRead);
    }

    SECTION("Get entry - invalid ID")
    {
        auto callEntry = callsTbl.GetByID(100);
        REQUIRE(callEntry.ID == DB_ID_NONE);
        REQUIRE(callEntry.number == "");
        REQUIRE(callEntry.e164number == "");
        REQUIRE(callEntry.presentation == PresentationType::PR_UNKNOWN);
        REQUIRE(callEntry.date == 0);
        REQUIRE(callEntry.duration == 0);
        REQUIRE(callEntry.type == CallType::CT_NONE);
        REQUIRE(callEntry.name == "");
        REQUIRE(callEntry.contactId == "");
        REQUIRE(callEntry.isRead == true);
    }

    SECTION("Get table rows")
    {
        // Get table rows using valid offset/limit parameters
        auto retOffsetLimit = callsTbl.GetLimitOffset(0, 4);
        REQUIRE(retOffsetLimit.size() == 4);

        // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
        auto retOffsetLimitBigger = callsTbl.GetLimitOffset(0, 100);
        REQUIRE(retOffsetLimitBigger.size() == 4);

        // Get table rows using invalid offset/limit parameters(should return empty object)
        auto retOffsetLimitFailed = callsTbl.GetLimitOffset(5, 4);
        REQUIRE(retOffsetLimitFailed.size() == 0);
    }

    SECTION("Remove entries")
    {
        REQUIRE(callsTbl.RemoveByID(2));

        // Table should have now 3 elements
        REQUIRE(callsTbl.GetCount() == 3);

        // Remove non existing element
        REQUIRE(callsTbl.RemoveByID(100));

        // Remove all elements from table
        REQUIRE(callsTbl.RemoveByID(1));
        REQUIRE(callsTbl.RemoveByID(3));
        REQUIRE(callsTbl.RemoveByID(4));

        // Table should be empty now
        REQUIRE(callsTbl.GetCount() == 0);
    }

    Database::Deinitialize();
}
