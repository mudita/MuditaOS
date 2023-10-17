// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "Helpers.hpp"

#include "Database/Database.hpp"
#include "module-db/databases/SmsDB.hpp"

#include <algorithm>
#include <filesystem>
#include <cstring>
#include <iostream>

TEST_CASE("SMS Table tests")
{
    db::tests::DatabaseUnderTest<SmsDB> smsDb{"sms.db", db::tests::getPurePhoneScriptsPath()};

    SMSTableRow testRow1 = {Record(0),
                            .threadID  = 0,
                            .contactID = 0,
                            .date      = 0,
                            .errorCode = 0,
                            .body      = "Test SMS message 1",
                            .type      = SMSType::INBOX

    };

    SMSTableRow testRow2 = {Record(0),
                            .threadID  = 0,
                            .contactID = 0,
                            .date      = 0,
                            .errorCode = 0,
                            .body      = "Test Draft SMS",
                            .type      = SMSType::DRAFT

    };

    const auto smsCount = smsDb.get().sms.count() + 1;
    // clear sms table
    for (std::uint32_t id = 1; id <= smsCount; id++) {
        REQUIRE(smsDb.get().sms.removeById(id));
    }

    REQUIRE(smsDb.get().sms.count() == 0);
    // add 4 elements into table
    REQUIRE(smsDb.get().sms.add(testRow1));
    REQUIRE(smsDb.get().sms.add(testRow1));
    REQUIRE(smsDb.get().sms.add(testRow1));
    REQUIRE(smsDb.get().sms.add(testRow1));

    // Table should have 4 elements
    REQUIRE(smsDb.get().sms.count() == 4);

    // update existing element in table
    testRow1.ID   = 4;
    testRow1.body = "updated Test SMS message ";
    REQUIRE(smsDb.get().sms.update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = smsDb.get().sms.getById(4);
    REQUIRE(sms.body == testRow1.body);

    // Get table row using invalid ID(should return empty smsDb.get().smsRow)
    auto smsFailed = smsDb.get().sms.getById(100);
    REQUIRE(smsFailed.body == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = smsDb.get().sms.getLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(smsDb.get().sms.getLimitOffsetByField(0, 4, SMSTableFields::Date, "0").size() == 4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = smsDb.get().sms.getLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = smsDb.get().sms.getLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(smsDb.get().sms.countByFieldId("thread_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(smsDb.get().sms.countByFieldId("invalid_field", 0) == 0);

    REQUIRE(smsDb.get().sms.removeById(2));

    // Table should have now 3 elements
    REQUIRE(smsDb.get().sms.count() == 3);

    // Remove non existing element
    REQUIRE(smsDb.get().sms.removeById(100));

    // Remove all elements from table
    REQUIRE(smsDb.get().sms.removeById(1));
    REQUIRE(smsDb.get().sms.removeById(3));
    REQUIRE(smsDb.get().sms.removeById(4));

    // Table should be empty now
    REQUIRE(smsDb.get().sms.count() == 0);

    SECTION("SMS Draft and Input Table test")
    {
        REQUIRE(smsDb.get().sms.add(testRow1));
        REQUIRE(smsDb.get().sms.add(testRow1));
        REQUIRE(smsDb.get().sms.add(testRow2));

        REQUIRE(smsDb.get().sms.countWithoutDraftsByThreadId(0) == 2);
        REQUIRE(smsDb.get().sms.count() == 3);

        REQUIRE(smsDb.get().sms.getDraftByThreadId(0).body == "Test Draft SMS");

        auto results = smsDb.get().sms.getByThreadIdWithoutDraftWithEmptyInput(0, 0, 10);

        REQUIRE(results.size() == 3);
        REQUIRE(results.back().type == SMSType::INPUT);
    }
}
