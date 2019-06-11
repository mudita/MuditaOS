
/*
 * @file ContactsNameTable_tests.cpp
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


TEST_CASE("Contacts Name Table tests")
{
    Database::Initialize();

    vfs.remove(ContactsDB::GetDBName());

    ContactsDB contactsdb;

    ContactsNameTableRow testRow1 = {.ID=0,
            .contactID=0,
            .namePrimary="Mateusz",
            .nameAlternative="Pati"
    };

    // Add 4 elements into table
    REQUIRE(contactsdb.name.Add(testRow1));
    REQUIRE(contactsdb.name.Add(testRow1));
    REQUIRE(contactsdb.name.Add(testRow1));
    REQUIRE(contactsdb.name.Add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsdb.name.GetCount() == 4);

    // Update existing element in table
    testRow1.ID = 4;
    testRow1.nameAlternative = "Pateusz";
    REQUIRE(contactsdb.name.Update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsdb.name.GetByID(4);
    REQUIRE(sms.nameAlternative == testRow1.nameAlternative);

    // Get table row using invalid ID(should return empty contactsdb.nameRow)
    auto smsFailed = contactsdb.name.GetByID(100);
    REQUIRE(smsFailed.nameAlternative  == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsdb.name.GetLimitOffset(0,4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsdb.name.GetLimitOffsetByField(0,4,ContactNameTableFields::NamePrimary,"Mateusz").size() == 4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsdb.name.GetLimitOffset(0,100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsdb.name.GetLimitOffset(5,4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsdb.name.GetCountByFieldID("contact_id",0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsdb.name.GetCountByFieldID("invalid_field",0) == 0);


    REQUIRE(contactsdb.name.RemoveByID(2));

    // Table should have now 3 elements
    REQUIRE(contactsdb.name.GetCount() == 3);

    // Remove non existing element
    REQUIRE(contactsdb.name.RemoveByID(100));

    // Remove all elements from table
    REQUIRE(contactsdb.name.RemoveByID(1));
    REQUIRE(contactsdb.name.RemoveByID(3));
    REQUIRE(contactsdb.name.RemoveByID(4));

    // Table should be empty now
    REQUIRE(contactsdb.name.GetCount() == 0);

    Database::Deinitialize();
}