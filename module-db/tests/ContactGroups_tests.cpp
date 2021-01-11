// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <Databases/ContactsDB.hpp>
#include <Tables/ContactsTable.hpp>
#include <Tables/ContactsGroups.hpp>

#include <filesystem>

#include <iomanip>
#include <sstream>
#include <purefs/filesystem_paths.hpp>

namespace consts
{
    uint32_t favouritesId = 1;
    uint32_t iceId        = 2;
    uint32_t blocked      = 3;
    uint32_t temporaryId  = 4;
} // namespace consts

void addSomeContacts(ContactsDB &contactsDb);

TEST_CASE("Contact Groups tests", "[Groups]")
{
    INFO("sqlite Init");
    Database::initialize();
    const auto callogPath = (purefs::dir::getUserDiskPath() / "contacts.db").c_str();
    std::filesystem::remove(callogPath);
    ContactsDB contactDb{callogPath};
    INFO("contactDB init");
    REQUIRE(contactDb.isInitialized());
    ContactsGroupsTable contactGroupsTable = ContactsGroupsTable(&contactDb);
    INFO("Create groups table");
    REQUIRE(contactGroupsTable.create());

    SECTION("Adding checking standard groups")
    {
        REQUIRE(contactGroupsTable.count() == 4);
        REQUIRE(contactGroupsTable.favouritesId() == consts::favouritesId);
        REQUIRE(contactGroupsTable.iceId() == consts::iceId);
        REQUIRE(contactGroupsTable.blockedId() == consts::blocked);
        REQUIRE(contactGroupsTable.temporaryId() == consts::temporaryId);
    }

    SECTION("CRUD")
    {
        const std::string sendbajtName("#sendbajt");
        const std::string sendbajt2Name("send'bajt");
        const std::string sendbajtWrongName("sentbyte");
        const std::string sendbajtProperName("sendbajt");

        // test # in name
        ContactsGroupsTableRow sendbajtGroup(sendbajtName);
        REQUIRE(contactGroupsTable.add(sendbajtGroup));
        REQUIRE(contactGroupsTable.getId(sendbajtName) != 0);

        // test ' in name
        ContactsGroupsTableRow sendbajt2Group(sendbajt2Name);
        REQUIRE(contactGroupsTable.add(sendbajt2Group));
        REQUIRE(contactGroupsTable.getId(sendbajt2Name) != 0);

        // add wrong naame
        ContactsGroupsTableRow wrongNameGroup(sendbajtWrongName);
        REQUIRE(contactGroupsTable.add(wrongNameGroup));
        ContactsGroupsTableRow newNameGroup(sendbajtProperName);
        newNameGroup.ID = contactGroupsTable.getId(sendbajtWrongName);

        // update group name
        REQUIRE(contactGroupsTable.update(newNameGroup));
        REQUIRE(contactGroupsTable.getId(sendbajtProperName) == newNameGroup.ID);
        REQUIRE(contactGroupsTable.getById(newNameGroup.ID).name == sendbajtProperName);

        // test remove statement
        uint32_t currentItemsCount = contactGroupsTable.count();
        REQUIRE(contactGroupsTable.removeById(newNameGroup.ID));

        // test if removing works
        REQUIRE(contactGroupsTable.count() == --currentItemsCount);
        REQUIRE(contactGroupsTable.getId(newNameGroup.name) == 0);
        REQUIRE(contactGroupsTable.getById(newNameGroup.ID).name.empty());

        // adding more items to check offset and limit works;

        for (unsigned int i = 0; i < 100; ++i) {
            std::stringstream name;
            name << "group_" << std::setw(3) << std::setfill('0') << i;
            ContactsGroupsTableRow row(name.str());
            contactGroupsTable.add(row);
            currentItemsCount++;
        }

        REQUIRE(contactGroupsTable.count() == currentItemsCount);
        REQUIRE(contactGroupsTable.getLimitOffset(0, 10).size() == 10);
        REQUIRE(contactGroupsTable.getLimitOffset(0, 150).size() == currentItemsCount);
        REQUIRE(contactGroupsTable.getAllRows().size() == currentItemsCount);

        uint32_t group_000_id = contactGroupsTable.getId("group_000");
        REQUIRE(group_000_id == 7);

        auto someGroups = contactGroupsTable.getLimitOffset(7, 10);
        REQUIRE(someGroups.size() == 10);
        REQUIRE(someGroups[0].name == "group_001");
        REQUIRE(someGroups[9].name == "group_010");
        REQUIRE(someGroups[9].ID == 17);

        INFO("Adding some contacts");
        addSomeContacts(contactDb);

        // adding to Favorites
        REQUIRE(contactGroupsTable.addContactToGroup(1, contactGroupsTable.favouritesId()));
        REQUIRE(contactGroupsTable.addContactToGroup(2, contactGroupsTable.favouritesId()));

        // adding to ICE
        REQUIRE(contactGroupsTable.addContactToGroup(1, contactGroupsTable.iceId()));
        REQUIRE(contactGroupsTable.addContactToGroup(2, contactGroupsTable.iceId()));
        REQUIRE(contactGroupsTable.addContactToGroup(3, contactGroupsTable.iceId()));

        // add to blocked
        REQUIRE(contactGroupsTable.addContactToGroup(4, contactGroupsTable.blockedId()));

        // check Favorites
        std::set<ContactsGroupsTableRow> groupsFor1 = contactGroupsTable.getGroupsForContact(1);
        REQUIRE(groupsFor1.size() == 2);

        std::set<ContactsGroupsTableRow> groupsFor2 = contactGroupsTable.getGroupsForContact(2);
        REQUIRE(groupsFor2.size() == 2);

        // getting all cantacts for group ICE
        std::set<uint32_t> iceContacts = contactGroupsTable.getContactsForGroup(contactGroupsTable.iceId());
        REQUIRE(iceContacts.size() == 3);

        // remove Contact From ICE
        REQUIRE(contactGroupsTable.removeContactFromGroup(2, contactGroupsTable.iceId()));

        // check if removing sucessful
        iceContacts = contactGroupsTable.getContactsForGroup(contactGroupsTable.iceId());
        REQUIRE(iceContacts.size() == 2);
        groupsFor2 = contactGroupsTable.getGroupsForContact(2);
        REQUIRE(groupsFor2.size() == 1);
    }

    SECTION("Update Groups")
    {
        addSomeContacts(contactDb);

        ContactsGroupsTableRow favouritesGroup(contactGroupsTable.getById(contactDb.favouritesGroupId()));
        ContactsGroupsTableRow iceGroup(contactGroupsTable.getById(contactDb.iceGroupId()));
        ContactsGroupsTableRow blockedGroup(contactGroupsTable.getById(contactDb.blockedGroupId()));
        ContactsGroupsTableRow familyGroup("Family");
        ContactsGroupsTableRow friendsGroup("Friends");
        ContactsGroupsTableRow workGroup("Work");

        // adding extra groups db
        contactGroupsTable.add(familyGroup);
        familyGroup.ID = contactGroupsTable.getId(familyGroup.name);
        contactGroupsTable.add(friendsGroup);
        friendsGroup.ID = contactGroupsTable.getId(friendsGroup.name);
        contactGroupsTable.add(workGroup);
        workGroup.ID = contactGroupsTable.getId(workGroup.name);

        // adding contact to some groups
        contactGroupsTable.addContactToGroup(1, contactDb.favouritesGroupId());
        contactGroupsTable.addContactToGroup(1, contactDb.iceGroupId());
        contactGroupsTable.addContactToGroup(1, contactDb.blockedGroupId());
        contactGroupsTable.addContactToGroup(1, familyGroup.ID);
        contactGroupsTable.addContactToGroup(1, workGroup.ID);

        // checking if adding was sucessfull
        REQUIRE(contactGroupsTable.getGroupsForContact(1).size() == 5);

        // creating new groups set
        std::set<ContactsGroupsTableRow> newGrupsSet;
        newGrupsSet.insert(blockedGroup);
        newGrupsSet.insert(familyGroup);
        newGrupsSet.insert(workGroup);
        newGrupsSet.insert(friendsGroup);

        // this function is tested
        contactGroupsTable.updateGroups(1, newGrupsSet);

        // getting new grups
        auto checkGroupsSet = contactGroupsTable.getGroupsForContact(1);
        auto checkEnd       = checkGroupsSet.end();

        // checking if update was successfull
        REQUIRE(checkGroupsSet.size() == 4);
        REQUIRE(checkGroupsSet.find(blockedGroup) != checkEnd);
        REQUIRE(checkGroupsSet.find(familyGroup) != checkEnd);
        REQUIRE(checkGroupsSet.find(workGroup) != checkEnd);
        REQUIRE(checkGroupsSet.find(friendsGroup) != checkEnd);

        REQUIRE(checkGroupsSet.find(favouritesGroup) == checkEnd);
        REQUIRE(checkGroupsSet.find(iceGroup) == checkEnd);
    }

    Database::deinitialize();
}

void addSomeContacts(ContactsDB &contactsDb)
{
    ContactsTableRow testRow1 = {
        {.ID = 0}, .nameID = 0, .numbersID = "0 1 2 3 4", .ringID = 0, .addressID = 0, .speedDial = "666"

    };

    // add 4 elements into table
    contactsDb.contacts.add(testRow1);
    contactsDb.contacts.add(testRow1);
    contactsDb.contacts.add(testRow1);
    contactsDb.contacts.add(testRow1);
}
