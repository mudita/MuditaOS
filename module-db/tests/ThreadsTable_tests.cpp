
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

#include "Tables/ThreadsTable.hpp"

TEST_CASE("Threads Table tests")
{
    Database::Initialize();

    vfs.remove(ThreadsTable::GetDBName());

    ThreadsTable threadstable;

    ThreadsTableRow testRow1 = {.ID=0,
            .date=0,
            .msgCount=0,
            .msgRead=0,
            .contactID=0,
            .snippet="Test snippet",
            .type = SMSType ::DRAFT

    };

    // Add 4 elements into table
    REQUIRE(threadstable.Add(testRow1));
    REQUIRE(threadstable.Add(testRow1));
    REQUIRE(threadstable.Add(testRow1));
    REQUIRE(threadstable.Add(testRow1));

    // Table should have 4 elements
    REQUIRE(threadstable.GetCount() == 4);

    // Update existing element in table
    testRow1.ID = 4;
    testRow1.snippet = "Updated Test snippet";
    REQUIRE(threadstable.Update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto thread = threadstable.GetByID(4);
    REQUIRE(thread.snippet == testRow1.snippet);

    // Get table row using invalid ID(should return empty SMSTableRow)
    auto threadFailed = threadstable.GetByID(100);
    REQUIRE(threadFailed.snippet  == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = threadstable.GetLimitOffset(0,4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = threadstable.GetLimitOffset(0,100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = threadstable.GetLimitOffset(5,4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    REQUIRE(threadstable.RemoveByID(2));

    // Table should have now 3 elements
    REQUIRE(threadstable.GetCount() == 3);

    // Remove non existing element
    REQUIRE(threadstable.RemoveByID(100));

    // Remove all elements from table
    REQUIRE(threadstable.RemoveByID(1));
    REQUIRE(threadstable.RemoveByID(3));
    REQUIRE(threadstable.RemoveByID(4));

    // Table should have be empty now
    REQUIRE(threadstable.GetCount() == 0);

    Database::Deinitialize();
}