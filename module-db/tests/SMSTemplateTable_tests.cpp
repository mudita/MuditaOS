// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/SmsDB.hpp"

#include "Tables/SMSTemplateTable.hpp"

#include <filesystem>

#include <algorithm>
#include <string>

#include <cstdint>

TEST_CASE("SMS Templates Table tests")
{
    Database::initialize();

    const auto smsPath = (std::filesystem::path{"sys/user"} / "sms.db");
    if (std::filesystem::exists(smsPath)) {
        REQUIRE(std::filesystem::remove(smsPath));
    }

    SmsDB smsDb{smsPath.c_str()};
    REQUIRE(smsDb.isInitialized());

    auto &templatesTbl = smsDb.templates;

    SMSTemplateTableRow testRow = {Record(0), .text = "Test text", .lastUsageTimestamp = 100};

    const auto templatesCount = templatesTbl.count() + 1;
    // clear sms table
    for (std::uint32_t id = 1; id <= templatesCount; id++) {
        REQUIRE(templatesTbl.removeById(id));
    }
    // add 4 elements into table
    REQUIRE(templatesTbl.add(testRow));
    REQUIRE(templatesTbl.add(testRow));
    REQUIRE(templatesTbl.add(testRow));
    REQUIRE(templatesTbl.add(testRow));

    REQUIRE(templatesTbl.count() == 4);

    SECTION("Get entry by ID")
    {
        auto templ = templatesTbl.getById(4);
        REQUIRE(templ.ID == 4);
        REQUIRE(templ.text == testRow.text);
        REQUIRE(templ.lastUsageTimestamp == testRow.lastUsageTimestamp);
    }

    SECTION("Entry update")
    {
        testRow.ID                 = 4;
        testRow.text               = "New text";
        testRow.lastUsageTimestamp = 200;
        REQUIRE(templatesTbl.update(testRow));
        auto templ = templatesTbl.getById(4);
        REQUIRE(templ.ID == 4);
        REQUIRE(templ.text == testRow.text);
        REQUIRE(templ.lastUsageTimestamp == testRow.lastUsageTimestamp);
    }

    SECTION("Get entry - invalid ID")
    {
        auto templ = templatesTbl.getById(100);
        REQUIRE(templ.ID == DB_ID_NONE);
        REQUIRE(templ.text == "");
        REQUIRE(templ.lastUsageTimestamp == 0);
    }

    SECTION("Get table rows")
    {
        // Get table rows using valid offset/limit parameters
        auto retOffsetLimit = templatesTbl.getLimitOffset(0, 4);
        REQUIRE(retOffsetLimit.size() == 4);

        // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
        auto retOffsetLimitBigger = templatesTbl.getLimitOffset(0, 100);
        REQUIRE(retOffsetLimitBigger.size() == 4);

        // Get table rows using invalid offset/limit parameters(should return empty object)
        auto retOffsetLimitFailed = templatesTbl.getLimitOffset(5, 4);
        REQUIRE(retOffsetLimitFailed.size() == 0);
    }

    SECTION("Remove entries")
    {
        REQUIRE(templatesTbl.removeById(2));

        // Table should have now 3 elements
        REQUIRE(templatesTbl.count() == 3);

        // Remove non existing element
        REQUIRE(templatesTbl.removeById(100));

        // Remove all elements from table
        REQUIRE(templatesTbl.removeById(1));
        REQUIRE(templatesTbl.removeById(3));
        REQUIRE(templatesTbl.removeById(4));

        // Table should be empty now
        REQUIRE(templatesTbl.count() == 0);
    }

    Database::deinitialize();
}
