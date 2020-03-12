
/*
 * @file ThreadsTable_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "vfs.hpp"

#include "catch.hpp"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <iostream>

#include "../Database/Database.hpp"
#include "../Databases/SmsDB.hpp"

#include "Tables/ThreadsTable.hpp"

TEST_CASE("Threads Table tests")
{
    Database::Initialize();

    vfs.remove(SmsDB::GetDBName());

    SmsDB smsdb;

    ThreadsTableRow testRow1 = {.ID        = 0,
                                .date      = 0,
                                .msgCount  = 0,
                                .msgRead   = 0,
                                .contactID = 0,
                                .snippet   = "Test snippet",
                                .type      = SMSType ::DRAFT

    };

    // Add 4 elements into table
    REQUIRE(smsdb.threads.Add(testRow1));
    REQUIRE(smsdb.threads.Add(testRow1));
    REQUIRE(smsdb.threads.Add(testRow1));
    REQUIRE(smsdb.threads.Add(testRow1));

    // Table should have 4 elements
    REQUIRE(smsdb.threads.GetCount() == 4);

    // Update existing element in table
    testRow1.ID      = 4;
    testRow1.snippet = "Updated Test snippet";
    REQUIRE(smsdb.threads.Update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto thread = smsdb.threads.GetByID(4);
    REQUIRE(thread.snippet == testRow1.snippet);

    // Get table row using invalid ID(should return empty SMSTableRow)
    auto threadFailed = smsdb.threads.GetByID(100);
    REQUIRE(threadFailed.snippet == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = smsdb.threads.GetLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = smsdb.threads.GetLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = smsdb.threads.GetLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(smsdb.threads.GetLimitOffsetByField(0, 4, ThreadsTableFields::MsgCount, "0").size() == 4);

    // Get count of elements by field's ID
    REQUIRE(smsdb.threads.GetCountByFieldID("contact_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(smsdb.threads.GetCountByFieldID("invalid_field", 0) == 0);

    REQUIRE(smsdb.threads.RemoveByID(2));

    // Table should have now 3 elements
    REQUIRE(smsdb.threads.GetCount() == 3);

    // Remove non existing element
    REQUIRE(smsdb.threads.RemoveByID(100));

    // Remove all elements from table
    REQUIRE(smsdb.threads.RemoveByID(1));
    REQUIRE(smsdb.threads.RemoveByID(3));
    REQUIRE(smsdb.threads.RemoveByID(4));

    // Table should be empty now
    REQUIRE(smsdb.threads.GetCount() == 0);

    Database::Deinitialize();
}