
/*
 * @file ContactsNumberTable_tests.cpp
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
#include "../Databases/ContactsDB.hpp"

TEST_CASE("Contacts Number Table tests")
{
    Database::Initialize();

    vfs.remove(ContactsDB::GetDBName());

    ContactsDB contactsdb;
    REQUIRE(contactsdb.IsInitialized());

    ContactsNumberTableRow testRow1 = {.ID = 0, .contactID = 0, .numberUser = "111222333", .numbere164 = "333222111"};

    // Add 4 elements into table
    REQUIRE(contactsdb.number.Add(testRow1));
    REQUIRE(contactsdb.number.Add(testRow1));
    REQUIRE(contactsdb.number.Add(testRow1));
    REQUIRE(contactsdb.number.Add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsdb.number.GetCount() == 4);

    // Update existing element in table
    testRow1.ID         = 4;
    testRow1.numberUser = "999888777";
    REQUIRE(contactsdb.number.Update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsdb.number.GetByID(4);
    REQUIRE(sms.numberUser == testRow1.numberUser);

    // Get table row using invalid ID(should return empty contactsdb.numberRow)
    auto smsFailed = contactsdb.number.GetByID(100);
    REQUIRE(smsFailed.numberUser == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsdb.number.GetLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsdb.number.GetLimitOffsetByField(0, 4, ContactNumberTableFields::NumberE164, "333222111").size() ==
            4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsdb.number.GetLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsdb.number.GetLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsdb.number.GetCountByFieldID("contact_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsdb.number.GetCountByFieldID("invalid_field", 0) == 0);

    REQUIRE(contactsdb.number.RemoveByID(2));

    // Table should have now 3 elements
    REQUIRE(contactsdb.number.GetCount() == 3);

    // Remove non existing element
    REQUIRE(contactsdb.number.RemoveByID(100));

    // Remove all elements from table
    REQUIRE(contactsdb.number.RemoveByID(1));
    REQUIRE(contactsdb.number.RemoveByID(3));
    REQUIRE(contactsdb.number.RemoveByID(4));

    // Table should be empty now
    REQUIRE(contactsdb.number.GetCount() == 0);

    Database::Deinitialize();
}
