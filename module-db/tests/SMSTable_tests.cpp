// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/SmsDB.hpp"

#include <algorithm>
#include <filesystem>
#include <cstring>
#include <iostream>

TEST_CASE("SMS Table tests", "[!mayfail]")
{
    Database::initialize();

    const auto smsPath = (std::filesystem::path{"sys/user"} / "sms.db");

    if (std::filesystem::exists(smsPath)) {
        REQUIRE(std::filesystem::remove(smsPath));
    }

    SmsDB smsdb(smsPath.c_str());
    REQUIRE(smsdb.isInitialized());

    SMSTableRow testRow1 = {Record(0),
                            .threadID  = 0,
                            .contactID = 0,
                            .date      = 0,
                            .errorCode = 0,
                            .body      = "Test SMS message 1",
                            .type      = SMSType ::INBOX

    };

    SMSTableRow testRow2 = {Record(0),
                            .threadID  = 0,
                            .contactID = 0,
                            .date      = 0,
                            .errorCode = 0,
                            .body      = "Test Draft SMS",
                            .type      = SMSType ::DRAFT

    };

    const auto smsCount = smsdb.sms.count() + 1;
    // clear sms table
    for (std::uint32_t id = 1; id <= smsCount; id++) {
        REQUIRE(smsdb.sms.removeById(id));
    }

    REQUIRE(smsdb.sms.count() == 0);
    // add 4 elements into table
    REQUIRE(smsdb.sms.add(testRow1));
    REQUIRE(smsdb.sms.add(testRow1));
    REQUIRE(smsdb.sms.add(testRow1));
    REQUIRE(smsdb.sms.add(testRow1));

    // Table should have 4 elements
    REQUIRE(smsdb.sms.count() == 4);

    // update existing element in table
    testRow1.ID   = 4;
    testRow1.body = "updated Test SMS message ";
    REQUIRE(smsdb.sms.update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = smsdb.sms.getById(4);
    REQUIRE(sms.body == testRow1.body);

    // Get table row using invalid ID(should return empty smsdb.smsRow)
    auto smsFailed = smsdb.sms.getById(100);
    REQUIRE(smsFailed.body == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = smsdb.sms.getLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(smsdb.sms.getLimitOffsetByField(0, 4, SMSTableFields::Date, "0").size() == 4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = smsdb.sms.getLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = smsdb.sms.getLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(smsdb.sms.countByFieldId("thread_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(smsdb.sms.countByFieldId("invalid_field", 0) == 0);

    REQUIRE(smsdb.sms.removeById(2));

    // Table should have now 3 elements
    REQUIRE(smsdb.sms.count() == 3);

    // Remove non existing element
    REQUIRE(smsdb.sms.removeById(100));

    // Remove all elements from table
    REQUIRE(smsdb.sms.removeById(1));
    REQUIRE(smsdb.sms.removeById(3));
    REQUIRE(smsdb.sms.removeById(4));

    // Table should be empty now
    REQUIRE(smsdb.sms.count() == 0);

    SECTION("SMS Draft and Input Table test")
    {
        REQUIRE(smsdb.sms.add(testRow1));
        REQUIRE(smsdb.sms.add(testRow1));
        REQUIRE(smsdb.sms.add(testRow2));

        REQUIRE(smsdb.sms.countWithoutDraftsByThreadId(0) == 2);
        REQUIRE(smsdb.sms.count() == 3);

        REQUIRE(smsdb.sms.getDraftByThreadId(0).body == "Test Draft SMS");

        auto results = smsdb.sms.getByThreadIdWithoutDraftWithEmptyInput(0, 0, 10);

        REQUIRE(results.size() == 3);
        REQUIRE(results.back().type == SMSType ::INPUT);
    }

    Database::deinitialize();
}
