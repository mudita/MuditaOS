#include "../Databases/ContactsDB.hpp"
#include "vfs.hpp"

#include "catch.hpp"

TEST_CASE("Contacts Address Table tests")
{
    Database::Initialize();

    vfs.remove(ContactsDB::GetDBName());

    ContactsDB contactsdb;
    REQUIRE(contactsdb.IsInitialized());

    ContactsAddressTableRow testRow1 = {.ID           = 0,
                                        .contactID    = 0,
                                        .addressLine1 = "6 Czeczota St.",
                                        .addressLine2 = "02600 Warsaw",
                                        .note         = "Test note",
                                        .mail         = "test@mudita.com"};

    // Add 4 elements into table
    REQUIRE(contactsdb.address.Add(testRow1));
    REQUIRE(contactsdb.address.Add(testRow1));
    REQUIRE(contactsdb.address.Add(testRow1));
    REQUIRE(contactsdb.address.Add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsdb.address.GetCount() == 4);

    // Update existing element in table
    testRow1.ID   = 4;
    testRow1.note = "Modified test note";
    REQUIRE(contactsdb.address.Update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsdb.address.GetByID(4);
    REQUIRE(sms.note == testRow1.note);

    // Get table row using invalid ID(should return empty contactsdb.addressRow)
    auto smsFailed = contactsdb.address.GetByID(100);
    REQUIRE(smsFailed.note == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsdb.address.GetLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsdb.address.GetLimitOffsetByField(0, 4, ContactAddressTableFields::Mail, "test@mudita.com").size() ==
            4);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsdb.address.GetLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsdb.address.GetLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsdb.address.GetCountByFieldID("contact_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsdb.address.GetCountByFieldID("invalid_field", 0) == 0);

    // Remove existing element
    REQUIRE(contactsdb.address.RemoveByID(2));

    // Table should have now 3 elements
    REQUIRE(contactsdb.address.GetCount() == 3);

    // Remove non existing element
    REQUIRE(contactsdb.address.RemoveByID(100));

    // Remove all elements from table
    REQUIRE(contactsdb.address.RemoveByID(1));
    REQUIRE(contactsdb.address.RemoveByID(3));
    REQUIRE(contactsdb.address.RemoveByID(4));

    // Table should be empty now
    REQUIRE(contactsdb.address.GetCount() == 0);

    Database::Deinitialize();
}