// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Databases/ContactsDB.hpp"
#include <filesystem>

TEST_CASE("Contacts address Table tests")
{
    Database::initialize();

    const auto callogPath = (std::filesystem::path{"sys/user"} / "contacts.db");
    if (std::filesystem::exists(callogPath)) {
        REQUIRE(std::filesystem::remove(callogPath));
    }

    ContactsDB contactsdb{callogPath.c_str()};
    REQUIRE(contactsdb.isInitialized());

    ContactsAddressTableRow testRow1 = {Record(DB_ID_NONE),
                                        .contactID = 0,
                                        .address   = "6 Czeczota St.\n02600 Warsaw",
                                        .note      = "Test note",
                                        .mail      = "test@mudita.com"};

    const auto contactsCount = contactsdb.address.count() + 1;
    // clear contacts table
    for (std::uint32_t id = 1; id <= contactsCount; id++) {
        REQUIRE(contactsdb.address.removeById(id));
    }
    // add 4 elements into table
    REQUIRE(contactsdb.address.add(testRow1));
    REQUIRE(contactsdb.address.add(testRow1));
    REQUIRE(contactsdb.address.add(testRow1));
    REQUIRE(contactsdb.address.add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsdb.address.count() == 4);

    // update existing element in table
    testRow1.ID   = 4;
    testRow1.note = "Modified test note";
    REQUIRE(contactsdb.address.update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsdb.address.getById(4);
    REQUIRE(sms.note == testRow1.note);

    // Get table row using invalid ID(should return empty contactsdb.addressRow)
    auto smsFailed = contactsdb.address.getById(100);
    REQUIRE(smsFailed.note == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsdb.address.getLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsdb.address.getLimitOffsetByField(0, 4, ContactAddressTableFields::Mail, "test@mudita.com").size() ==
            4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsdb.address.getLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsdb.address.getLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsdb.address.countByFieldId("contact_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsdb.address.countByFieldId("invalid_field", 0) == 0);

    // Remove existing element
    REQUIRE(contactsdb.address.removeById(2));

    // Table should have now 3 elements
    REQUIRE(contactsdb.address.count() == 3);

    // Remove non existing element
    REQUIRE(contactsdb.address.removeById(100));

    // Remove all elements from table
    REQUIRE(contactsdb.address.removeById(1));
    REQUIRE(contactsdb.address.removeById(3));
    REQUIRE(contactsdb.address.removeById(4));

    // Table should be empty now
    REQUIRE(contactsdb.address.count() == 0);

    Database::deinitialize();
}
