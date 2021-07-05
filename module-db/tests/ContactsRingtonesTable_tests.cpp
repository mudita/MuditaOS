// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/ContactsDB.hpp"

#include <algorithm>
#include <filesystem>

#include <cstdint>
#include <cstdio>
#include <cstring>

TEST_CASE("Contacts Ringtones Table tests")
{
    Database::initialize();

    const auto contactsPath = (std::filesystem::path{"sys/user"} / "contacts.db");
    if (std::filesystem::exists(contactsPath)) {
        REQUIRE(std::filesystem::remove(contactsPath));
    }

    ContactsDB contactsdb{contactsPath.c_str()};
    REQUIRE(contactsdb.isInitialized());

    ContactsRingtonesTableRow testRow1(DB_ID_NONE, DB_ID_NONE, "/test/assets/path/ringtone.wr");

    const auto contactsCount = contactsdb.ringtones.count() + 1;
    // clear contacts table
    for (std::uint32_t id = 1; id <= contactsCount; id++) {
        REQUIRE(contactsdb.ringtones.removeById(id));
    }
    // add 4 elements into table
    REQUIRE(contactsdb.ringtones.add(testRow1));
    REQUIRE(contactsdb.ringtones.add(testRow1));
    REQUIRE(contactsdb.ringtones.add(testRow1));
    REQUIRE(contactsdb.ringtones.add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsdb.ringtones.count() == 4);

    // update existing element in table
    testRow1.ID        = 4;
    testRow1.assetPath = "/testnew/assets/path/ringtone2.wr";
    REQUIRE(contactsdb.ringtones.update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsdb.ringtones.getById(4);
    REQUIRE(sms.assetPath == testRow1.assetPath);

    // Get table row using invalid ID(should return empty contactsdb.ringtonesRow)
    auto smsFailed = contactsdb.ringtones.getById(100);
    REQUIRE(smsFailed.assetPath == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsdb.ringtones.getLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsdb.ringtones
                .getLimitOffsetByField(0, 4, ContactRingtonesTableFields::AssetPath, "/test/assets/path/ringtone.wr")
                .size() == 3);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsdb.ringtones.getLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsdb.ringtones.getLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsdb.ringtones.countByFieldId("contact_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsdb.ringtones.countByFieldId("invalid_field", 0) == 0);

    REQUIRE(contactsdb.ringtones.removeById(2));

    // Table should have now 3 elements
    REQUIRE(contactsdb.ringtones.count() == 3);

    // Remove non existing element
    REQUIRE(contactsdb.ringtones.removeById(100));

    // Remove all elements from table
    REQUIRE(contactsdb.ringtones.removeById(1));
    REQUIRE(contactsdb.ringtones.removeById(3));
    REQUIRE(contactsdb.ringtones.removeById(4));

    // Table should be empty now
    REQUIRE(contactsdb.ringtones.count() == 0);

    Database::deinitialize();
}
