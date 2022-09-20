// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "Helpers.hpp"
#include <filesystem>

#include "module-db/databases/ContactsDB.hpp"
#include "Tables/ContactsTable.hpp"

TEST_CASE("Contacts Table tests")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    ContactsTableRow testRow1 = {Record(DB_ID_NONE),
                                 .nameID    = DB_ID_NONE,
                                 .numbersID = "0 1 2 3 4",
                                 .ringID    = DB_ID_NONE,
                                 .addressID = DB_ID_NONE,
                                 .speedDial = "666"

    };
    REQUIRE(
        contactsDb.get().execute("INSERT OR REPLACE INTO  contact_name (_id,contact_id,name_primary,name_alternative) "
                                 "VALUES (1,1,'Alek','Wyczesany');"));
    REQUIRE(
        contactsDb.get().execute("INSERT OR REPLACE INTO  contact_name (_id,contact_id,name_primary,name_alternative) "
                                 "VALUES (2,2,'Zofia','Wyczesany');"));
    REQUIRE(
        contactsDb.get().execute("INSERT OR REPLACE INTO  contact_name (_id,contact_id,name_primary,name_alternative) "
                                 "VALUES (3,3,'Cezary','Wyczesany');"));
    REQUIRE(
        contactsDb.get().execute("INSERT OR REPLACE INTO  contact_name (_id,contact_id,name_primary,name_alternative) "
                                 "VALUES (4,4,'Alek','Arbuz');"));
    REQUIRE(contactsDb.get().execute(
        "INSERT OR REPLACE INTO  contact_match_groups (_id,group_id,contact_id) VALUES (1,1,1);"));
    REQUIRE(contactsDb.get().execute(
        "INSERT OR REPLACE INTO  contact_match_groups (_id,group_id,contact_id) VALUES (2,2,2);"));
    REQUIRE(contactsDb.get().execute(
        "INSERT OR REPLACE INTO  contact_match_groups (_id,group_id,contact_id) VALUES (3,1,3);"));
    REQUIRE(contactsDb.get().execute(
        "INSERT OR REPLACE INTO  contact_match_groups (_id,group_id,contact_id) VALUES (4,1,4);"));

    const auto contactsCount = contactsDb.get().contacts.count() + 1;
    // clear contacts table
    for (std::uint32_t id = 1; id <= contactsCount; id++) {
        REQUIRE(contactsDb.get().contacts.removeById(id));
    }
    // add 4 elements into table
    REQUIRE(contactsDb.get().contacts.add(testRow1));
    REQUIRE(contactsDb.get().contacts.add(testRow1));
    REQUIRE(contactsDb.get().contacts.add(testRow1));
    REQUIRE(contactsDb.get().contacts.add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsDb.get().contacts.count() == 4);

    // update existing element in table
    testRow1.ID        = 4;
    testRow1.speedDial = "777";
    REQUIRE(contactsDb.get().contacts.update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsDb.get().contacts.getById(4);
    REQUIRE(sms.speedDial == testRow1.speedDial);

    // Get table row using invalid ID(should return empty contactsDb.get().contactsRow)
    auto smsFailed = contactsDb.get().contacts.getById(100);
    REQUIRE(smsFailed.speedDial == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsDb.get().contacts.getLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsDb.get().contacts.getLimitOffsetByField(0, 4, ContactTableFields::SpeedDial, "666").size() == 3);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsDb.get().contacts.getLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    auto sortedRetOffsetLimitBigger =
        contactsDb.get().contacts.GetIDsSortedByField(ContactsTable::MatchType::Name, "", 1, 4, 0);
    REQUIRE(sortedRetOffsetLimitBigger.size() == 4);

    sortedRetOffsetLimitBigger = contactsDb.get().contacts.GetIDsSortedByName(1, 4);
    REQUIRE(sortedRetOffsetLimitBigger.size() == 1);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsDb.get().contacts.getLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsDb.get().contacts.countByFieldId("ring_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsDb.get().contacts.countByFieldId("invalid_field", 0) == 0);

    REQUIRE(contactsDb.get().contacts.removeById(2));

    // Table should have now 3 elements
    REQUIRE(contactsDb.get().contacts.count() == 3);

    // Remove non existing element
    REQUIRE(contactsDb.get().contacts.removeById(100));

    // Remove all elements from table
    REQUIRE(contactsDb.get().contacts.removeById(1));
    REQUIRE(contactsDb.get().contacts.removeById(3));
    REQUIRE(contactsDb.get().contacts.removeById(4));

    // Table should be empty now
    REQUIRE(contactsDb.get().contacts.count() == 0);
}
