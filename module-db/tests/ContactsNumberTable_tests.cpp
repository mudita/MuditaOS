// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "Helpers.hpp"

#include "module-db/databases/ContactsDB.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Contacts Number Table tests")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    ContactsNumberTableRow testRow1 = {
        Record(DB_ID_NONE), .contactID = DB_ID_NONE, .numberUser = "111222333", .numbere164 = "333222111"};

    const auto contactsCount = contactsDb.get().number.count() + 1;
    // clear contacts table
    for (std::uint32_t id = 1; id <= contactsCount; id++) {
        REQUIRE(contactsDb.get().number.removeById(id));
    }
    // add 4 elements into table
    REQUIRE(contactsDb.get().number.add(testRow1));
    REQUIRE(contactsDb.get().number.add(testRow1));
    REQUIRE(contactsDb.get().number.add(testRow1));
    REQUIRE(contactsDb.get().number.add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsDb.get().number.count() == 4);

    // update existing element in table
    testRow1.ID         = 4;
    testRow1.numberUser = "999888777";
    REQUIRE(contactsDb.get().number.update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsDb.get().number.getById(4);
    REQUIRE(sms.numberUser == testRow1.numberUser);

    // Get table row using invalid ID(should return empty contactsDb.get().numberRow)
    auto smsFailed = contactsDb.get().number.getById(100);
    REQUIRE(smsFailed.numberUser == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsDb.get().number.getLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(
        contactsDb.get().number.getLimitOffsetByField(0, 4, ContactNumberTableFields::NumberE164, "333222111").size() ==
        4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsDb.get().number.getLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsDb.get().number.getLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsDb.get().number.countByFieldId("contact_id", DB_ID_NONE) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsDb.get().number.countByFieldId("invalid_field", DB_ID_NONE) == 0);

    REQUIRE(contactsDb.get().number.removeById(2));

    // Table should have now 3 elements
    REQUIRE(contactsDb.get().number.count() == 3);

    // Remove non existing element
    REQUIRE(contactsDb.get().number.removeById(100));

    // Remove all elements from table
    REQUIRE(contactsDb.get().number.removeById(1));
    REQUIRE(contactsDb.get().number.removeById(3));
    REQUIRE(contactsDb.get().number.removeById(4));

    // Table should be empty now
    REQUIRE(contactsDb.get().number.count() == 0);
}
