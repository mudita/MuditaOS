
/*
 * @file ContactsRingtonesTable_tests.cpp
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

TEST_CASE("Contacts Ringtones Table tests")
{
    Database::Initialize();

    vfs.remove(ContactsDB::GetDBName());

    ContactsDB contactsdb;

    ContactsRingtonesTableRow testRow1 = {.ID=0,
            .contactID=0,
            .assetPath="/test/assets/path/ringtone.wr"
    };

    // Add 4 elements into table
    REQUIRE(contactsdb.ringtones.Add(testRow1));
    REQUIRE(contactsdb.ringtones.Add(testRow1));
    REQUIRE(contactsdb.ringtones.Add(testRow1));
    REQUIRE(contactsdb.ringtones.Add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsdb.ringtones.GetCount() == 4);

    // Update existing element in table
    testRow1.ID = 4;
    testRow1.assetPath = "/testnew/assets/path/ringtone2.wr";
    REQUIRE(contactsdb.ringtones.Update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsdb.ringtones.GetByID(4);
    REQUIRE(sms.assetPath == testRow1.assetPath);

    // Get table row using invalid ID(should return empty contactsdb.ringtonesRow)
    auto smsFailed = contactsdb.ringtones.GetByID(100);
    REQUIRE(smsFailed.assetPath  == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsdb.ringtones.GetLimitOffset(0,4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsdb.ringtones.GetLimitOffsetByFieldID(0,4,"contact_id", 0).size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsdb.ringtones.GetLimitOffsetByFieldID(0,4,"contact_id", 100).size() == 0);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsdb.ringtones.GetLimitOffset(0,100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsdb.ringtones.GetLimitOffset(5,4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsdb.ringtones.GetCountByFieldID("contact_id",0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsdb.ringtones.GetCountByFieldID("invalid_field",0) == 0);


    REQUIRE(contactsdb.ringtones.RemoveByID(2));

    // Table should have now 3 elements
    REQUIRE(contactsdb.ringtones.GetCount() == 3);

    // Remove non existing element
    REQUIRE(contactsdb.ringtones.RemoveByID(100));

    // Remove all elements from table
    REQUIRE(contactsdb.ringtones.RemoveByID(1));
    REQUIRE(contactsdb.ringtones.RemoveByID(3));
    REQUIRE(contactsdb.ringtones.RemoveByID(4));

    // Table should be empty now
    REQUIRE(contactsdb.ringtones.GetCount() == 0);

    Database::Deinitialize();
}