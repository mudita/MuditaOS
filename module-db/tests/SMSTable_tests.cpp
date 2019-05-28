
/*
 * @file SMSTable_tests.cpp
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

#include "Tables/SMSTable.hpp"

TEST_CASE("SMS Table tests")
{
    Database::Initialize();

    vfs.remove(SMSTable::GetDBName());

    SMSTable smstable;

    SMSTableRow testRow1 = {.ID=0,
            .threadID=0,
            .contactID=0,
            .date=0,
            .dateSent=0,
            .errorCode=0,
            .body="Test SMS message 1",
            .isRead=true,
            .type = SMSType ::INBOX

    };

    // Add 4 elements into table
    REQUIRE(smstable.Add(testRow1));
    REQUIRE(smstable.Add(testRow1));
    REQUIRE(smstable.Add(testRow1));
    REQUIRE(smstable.Add(testRow1));

    // Table should have 4 elements
    REQUIRE(smstable.GetCount() == 4);

    // Update existing element in table
    testRow1.ID = 4;
    testRow1.body = "Updated Test SMS message ";
    REQUIRE(smstable.Update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = smstable.GetByID(4);
    REQUIRE(sms.body == testRow1.body);

    // Get table row using invalid ID(should return empty SMSTableRow)
    auto smsFailed = smstable.GetByID(100);
    REQUIRE(smsFailed.body  == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = smstable.GetLimitOffset(0,4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = smstable.GetLimitOffset(0,100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = smstable.GetLimitOffset(5,4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    REQUIRE(smstable.RemoveByID(2));

    // Table should have now 3 elements
    REQUIRE(smstable.GetCount() == 3);

    // Remove non existing element
    REQUIRE(smstable.RemoveByID(100));

    // Remove all elements from table
    REQUIRE(smstable.RemoveByID(1));
    REQUIRE(smstable.RemoveByID(3));
    REQUIRE(smstable.RemoveByID(4));

    // Table should have be empty now
    REQUIRE(smstable.GetCount() == 0);

    Database::Deinitialize();
}