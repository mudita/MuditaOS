// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/CalllogDB.hpp"

#include "Tables/CalllogTable.hpp"

#include <cstdint>
#include <string>
#include <algorithm>
#include <filesystem>

TEST_CASE("Calllog Table tests")
{
    Database::initialize();

    const auto calllogPath = (std::filesystem::path{"sys/user"} / "calllog.db");
    if (std::filesystem::exists(calllogPath)) {
        REQUIRE(std::filesystem::remove(calllogPath));
    }

    CalllogDB calllogDb{calllogPath.c_str()};
    REQUIRE(calllogDb.isInitialized());

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
        REQUIRE(testRow.contactId == 0);
        REQUIRE(testRow.isRead == true);
    }

    CalllogTableRow testRow = {Record(DB_ID_NONE),
                               .number       = "600123456",
                               .e164number   = "+48600226908",
                               .presentation = PresentationType::PR_ALLOWED,
                               .date         = 0,
                               .duration     = 100,
                               .type         = CallType::CT_INCOMING,
                               .name         = "Test name",
                               .contactId    = 1,
                               .isRead       = false};

    const auto callsCount = callsTbl.count() + 1;
    // clear callog table
    for (std::size_t id = 1; id < callsCount; id++) {
        REQUIRE(callsTbl.removeById(id));
    }
    // add 4 elements into table
    REQUIRE(callsTbl.add(testRow));
    REQUIRE(callsTbl.add(testRow));
    REQUIRE(callsTbl.add(testRow));
    REQUIRE(callsTbl.add(testRow));

    REQUIRE(callsTbl.count() == 4);

    SECTION("Get entry by ID")
    {
        auto callEntry = callsTbl.getById(4);
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
        REQUIRE(callsTbl.update(testRow));
        auto callEntry = callsTbl.getById(3);
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
        auto callEntry = callsTbl.getById(100);
        REQUIRE(callEntry.ID == DB_ID_NONE);
        REQUIRE(callEntry.number == "");
        REQUIRE(callEntry.e164number == "");
        REQUIRE(callEntry.presentation == PresentationType::PR_UNKNOWN);
        REQUIRE(callEntry.date == 0);
        REQUIRE(callEntry.duration == 0);
        REQUIRE(callEntry.type == CallType::CT_NONE);
        REQUIRE(callEntry.name == "");
        REQUIRE(callEntry.contactId == 0);
        REQUIRE(callEntry.isRead == true);
    }

    SECTION("Get table rows")
    {
        // Get table rows using valid offset/limit parameters
        auto retOffsetLimit = callsTbl.getLimitOffset(0, 4);
        REQUIRE(retOffsetLimit.size() == 4);

        // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
        auto retOffsetLimitBigger = callsTbl.getLimitOffset(0, 100);
        REQUIRE(retOffsetLimitBigger.size() == 4);

        // Get table rows using invalid offset/limit parameters(should return empty object)
        auto retOffsetLimitFailed = callsTbl.getLimitOffset(5, 4);
        REQUIRE(retOffsetLimitFailed.size() == 0);
    }

    SECTION("Remove entries")
    {
        REQUIRE(callsTbl.removeById(2));

        // Table should have now 3 elements
        REQUIRE(callsTbl.count() == 3);

        // Remove non existing element
        REQUIRE(callsTbl.removeById(100));

        // Remove all elements from table
        REQUIRE(callsTbl.removeById(1));
        REQUIRE(callsTbl.removeById(3));
        REQUIRE(callsTbl.removeById(4));

        // Table should be empty now
        REQUIRE(callsTbl.count() == 0);
    }

    SECTION("Get Count")
    {
        REQUIRE(callsTbl.count() == 4);
        REQUIRE(callsTbl.count(EntryState::ALL) == 4);
        REQUIRE(callsTbl.count(EntryState::READ) == 0);
        REQUIRE(callsTbl.count(EntryState::UNREAD) == 4);
    }

    SECTION("Set All Read")
    {
        REQUIRE(callsTbl.count(EntryState::UNREAD) == 4);
        REQUIRE(callsTbl.count(EntryState::READ) == 0);
        REQUIRE(callsTbl.SetAllRead());
        REQUIRE(callsTbl.count(EntryState::UNREAD) == 0);
        REQUIRE(callsTbl.count(EntryState::READ) == 4);
    }

    Database::deinitialize();
}
