// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/SmsDB.hpp"
#include "Tables/ThreadsTable.hpp"

#include <algorithm>
#include <filesystem>

#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Threads Table tests")
{
    Database::initialize();

    const auto smsPath = (std::filesystem::path{"sys/user"} / "sms.db");
    if (std::filesystem::exists(smsPath)) {
        REQUIRE(std::filesystem::remove(smsPath));
    }

    SmsDB smsdb{smsPath.c_str()};
    REQUIRE(smsdb.isInitialized());

    ThreadsTableRow testRow1 = {Record(0),
                                .date           = 0,
                                .msgCount       = 0,
                                .unreadMsgCount = 0,
                                .contactID      = 0,
                                .snippet        = "Test snippet",
                                .type           = SMSType::DRAFT

    };

    const auto smsThreadsCount = smsdb.threads.count() + 1;
    // clear threads table
    for (std::size_t id = 1; id <= smsThreadsCount; id++) {
        REQUIRE(smsdb.threads.removeById(id));
    }

    // add 4 elements into table
    REQUIRE(smsdb.threads.add(testRow1));
    REQUIRE(smsdb.threads.add(testRow1));
    REQUIRE(smsdb.threads.add(testRow1));
    testRow1.unreadMsgCount = 10;
    REQUIRE(smsdb.threads.add(testRow1));

    // Table should have 4 elements
    REQUIRE(smsdb.threads.count() == 4);
    REQUIRE(smsdb.threads.count(EntryState::ALL) == 4);
    REQUIRE(smsdb.threads.count(EntryState::READ) == 3);
    REQUIRE(smsdb.threads.count(EntryState::UNREAD) == 1);

    // update existing element in table
    testRow1.ID      = 4;
    testRow1.snippet = "updated Test snippet";
    REQUIRE(smsdb.threads.update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto thread = smsdb.threads.getById(4);
    REQUIRE(thread.snippet == testRow1.snippet);

    // Get table row using invalid ID(should return empty SMSTableRow)
    auto threadFailed = smsdb.threads.getById(100);
    REQUIRE(threadFailed.snippet == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = smsdb.threads.getLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = smsdb.threads.getLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = smsdb.threads.getLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(smsdb.threads.getLimitOffsetByField(0, 4, ThreadsTableFields::MsgCount, "0").size() == 4);

    // Get count of elements by field's ID
    REQUIRE(smsdb.threads.countByFieldId("contact_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(smsdb.threads.countByFieldId("invalid_field", 0) == 0);

    REQUIRE(smsdb.threads.removeById(2));

    // Table should have now 3 elements
    REQUIRE(smsdb.threads.count() == 3);

    // Remove non existing element
    REQUIRE(smsdb.threads.removeById(100));

    // Remove all elements from table
    REQUIRE(smsdb.threads.removeById(1));
    REQUIRE(smsdb.threads.removeById(3));
    REQUIRE(smsdb.threads.removeById(4));

    // Table should be empty now
    REQUIRE(smsdb.threads.count() == 0);

    Database::deinitialize();
}
