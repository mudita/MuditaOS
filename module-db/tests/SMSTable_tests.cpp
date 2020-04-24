
/*
 * @file smsdb.sms_tests.cpp
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

#include "Tables/SMSTable.hpp"

TEST_CASE("SMS Table tests")
{
    Database::Initialize();

    vfs.remove(SmsDB::GetDBName());

    SmsDB smsdb;
    REQUIRE(smsdb.IsInitialized());

    SMSTableRow testRow1 = {.ID        = 0,
                            .threadID  = 0,
                            .contactID = 0,
                            .date      = 0,
                            .dateSent  = 0,
                            .errorCode = 0,
                            .body      = "Test SMS message 1",
                            .isRead    = true,
                            .type      = SMSType ::INBOX

    };

    // Add 4 elements into table
    REQUIRE(smsdb.sms.Add(testRow1));
    REQUIRE(smsdb.sms.Add(testRow1));
    REQUIRE(smsdb.sms.Add(testRow1));
    REQUIRE(smsdb.sms.Add(testRow1));

    // Table should have 4 elements
    REQUIRE(smsdb.sms.GetCount() == 4);

    // Update existing element in table
    testRow1.ID   = 4;
    testRow1.body = "Updated Test SMS message ";
    REQUIRE(smsdb.sms.Update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = smsdb.sms.GetByID(4);
    REQUIRE(sms.body == testRow1.body);

    // Get table row using invalid ID(should return empty smsdb.smsRow)
    auto smsFailed = smsdb.sms.GetByID(100);
    REQUIRE(smsFailed.body == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = smsdb.sms.GetLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(smsdb.sms.GetLimitOffsetByField(0, 4, SMSTableFields::Date, "0").size() == 4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = smsdb.sms.GetLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = smsdb.sms.GetLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(smsdb.sms.GetCountByFieldID("thread_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(smsdb.sms.GetCountByFieldID("invalid_field", 0) == 0);

    REQUIRE(smsdb.sms.RemoveByID(2));

    // Table should have now 3 elements
    REQUIRE(smsdb.sms.GetCount() == 3);

    // Remove non existing element
    REQUIRE(smsdb.sms.RemoveByID(100));

    // Remove all elements from table
    REQUIRE(smsdb.sms.RemoveByID(1));
    REQUIRE(smsdb.sms.RemoveByID(3));
    REQUIRE(smsdb.sms.RemoveByID(4));

    // Table should be empty now
    REQUIRE(smsdb.sms.GetCount() == 0);

    Database::Deinitialize();
}