// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <filesystem>

#include "Databases/ContactsDB.hpp"
#include "Tables/ContactsTable.hpp"

#include <vfs.hpp>
#include <purefs/filesystem_paths.hpp>

TEST_CASE("Contacts Table tests")
{
    Database::initialize();

    const auto contactsPath = purefs::dir::getUserDiskPath() / "contacts.db";
    std::filesystem::remove(contactsPath);

    ContactsDB contactsdb{contactsPath.c_str()};
    REQUIRE(contactsdb.isInitialized());

    ContactsTableRow testRow1 = {{.ID = DB_ID_NONE},
                                 .nameID    = DB_ID_NONE,
                                 .numbersID = "0 1 2 3 4",
                                 .ringID    = DB_ID_NONE,
                                 .addressID = DB_ID_NONE,
                                 .speedDial = "666"

    };

    // add 4 elements into table
    REQUIRE(contactsdb.contacts.add(testRow1));
    REQUIRE(contactsdb.contacts.add(testRow1));
    REQUIRE(contactsdb.contacts.add(testRow1));
    REQUIRE(contactsdb.contacts.add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsdb.contacts.count() == 4);

    // update existing element in table
    testRow1.ID        = 4;
    testRow1.speedDial = "777";
    REQUIRE(contactsdb.contacts.update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsdb.contacts.getById(4);
    REQUIRE(sms.speedDial == testRow1.speedDial);

    // Get table row using invalid ID(should return empty contactsdb.contactsRow)
    auto smsFailed = contactsdb.contacts.getById(100);
    REQUIRE(smsFailed.speedDial == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsdb.contacts.getLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsdb.contacts.getLimitOffsetByField(0, 4, ContactTableFields::SpeedDial, "666").size() == 3);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsdb.contacts.getLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    auto sortedRetOffsetLimitBigger =
        contactsdb.contacts.GetIDsSortedByField(ContactsTable::MatchType::Name, "", 1, 100, 0);
    REQUIRE(sortedRetOffsetLimitBigger.size() == 4);

    sortedRetOffsetLimitBigger = contactsdb.contacts.GetIDsSortedByName(1, 100);
    REQUIRE(sortedRetOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsdb.contacts.getLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsdb.contacts.countByFieldId("ring_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsdb.contacts.countByFieldId("invalid_field", 0) == 0);

    REQUIRE(contactsdb.contacts.removeById(2));

    // Table should have now 3 elements
    REQUIRE(contactsdb.contacts.count() == 3);

    // Remove non existing element
    REQUIRE(contactsdb.contacts.removeById(100));

    // Remove all elements from table
    REQUIRE(contactsdb.contacts.removeById(1));
    REQUIRE(contactsdb.contacts.removeById(3));
    REQUIRE(contactsdb.contacts.removeById(4));

    // Table should be empty now
    REQUIRE(contactsdb.contacts.count() == 0);

    Database::deinitialize();
}
