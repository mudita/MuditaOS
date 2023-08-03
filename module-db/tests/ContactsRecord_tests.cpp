// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common.hpp"
#include "Helpers.hpp"
#include <catch2/catch.hpp>

#include "Interface/ContactRecord.hpp"
#include <i18n/i18n.hpp>

TEST_CASE("Contact Record db tests")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    const char *primaryNameTest                   = "PrimaryNameTest";
    const char *alternativeNameTest               = "AlternativeNameTest";
    const char *numberUserTest                    = "600123456";
    const char *numberE164Test                    = "+48600123456";
    const char *addressTest                       = "6 Czeczota St.\n02600 Warsaw";
    const char *noteTest                          = "TestNote";
    const char *mailTest                          = "TestMail";
    const char *assetPath                         = "/Test/Path/To/Asset";
    const char *speeddialTest                     = "100";
    const ContactNumberType contactNumberTypeTest = ContactNumberType::PAGER;

    ContactRecordInterface contRecInterface(&contactsDb.get());

    ContactRecord recordIN;

    recordIN.primaryName     = primaryNameTest;
    recordIN.alternativeName = alternativeNameTest;
    recordIN.address         = addressTest;
    recordIN.note            = noteTest;
    recordIN.mail            = mailTest;
    recordIN.assetPath       = assetPath;
    recordIN.speeddial       = speeddialTest;

    recordIN.numbers = {ContactRecord::Number("600123451", "+48600123451", contactNumberTypeTest)};
    REQUIRE(contRecInterface.Add(recordIN));
    recordIN.numbers = {ContactRecord::Number("600123452", "+48600123452", contactNumberTypeTest)};
    REQUIRE(contRecInterface.Add(recordIN));
    recordIN.numbers = {ContactRecord::Number("600123453", "+48600123453", contactNumberTypeTest)};
    REQUIRE(contRecInterface.Add(recordIN));
    recordIN.numbers = {ContactRecord::Number("600123454", "+48600123454", contactNumberTypeTest)};
    REQUIRE(contRecInterface.Add(recordIN));
    recordIN.numbers = {ContactRecord::Number(numberUserTest, numberE164Test, contactNumberTypeTest)};
    REQUIRE(contRecInterface.Add(recordIN));
    REQUIRE(contRecInterface.GetCount() == 5);

    SECTION("Get record by ID")
    {
        auto recordOUT = contRecInterface.GetByID(5);
        REQUIRE(recordOUT.ID == 5);

        REQUIRE(recordOUT.primaryName == primaryNameTest);
        REQUIRE(recordOUT.alternativeName == alternativeNameTest);
        REQUIRE(recordOUT.numbers.size() == 1);
        REQUIRE(recordOUT.numbers[0].number.getEntered() == numberUserTest);
        REQUIRE(recordOUT.numbers[0].number.getE164() == numberE164Test);
        REQUIRE(recordOUT.numbers[0].numberType == contactNumberTypeTest);
        REQUIRE(recordOUT.address == addressTest);
        REQUIRE(recordOUT.note == noteTest);
        REQUIRE(recordOUT.mail == mailTest);
        REQUIRE(recordOUT.assetPath == assetPath);
        REQUIRE(recordOUT.speeddial == speeddialTest);
    }

    SECTION("Get records by limit offset by NumberE164 filed")
    {
        auto recordList = contRecInterface.GetLimitOffsetByField(0, 2, ContactRecordField::NumberE164, numberE164Test);
        REQUIRE(recordList->size() == 1);

        auto w = recordList->front();
        REQUIRE(w.ID == 5);
        REQUIRE(w.primaryName == primaryNameTest);
        REQUIRE(w.alternativeName == alternativeNameTest);
        REQUIRE(w.numbers[0].number.getEntered() == numberUserTest);
        REQUIRE(w.numbers[0].number.getE164() == numberE164Test);
        REQUIRE(w.numbers[0].numberType == contactNumberTypeTest);
        REQUIRE(w.address == addressTest);
        REQUIRE(w.note == noteTest);
        REQUIRE(w.mail == mailTest);
        REQUIRE(w.assetPath == assetPath);
        REQUIRE(w.speeddial == speeddialTest);
    }

    REQUIRE(contRecInterface.RemoveByID(5));

    SECTION("Get records by limit offset")
    {
        auto recordList = contRecInterface.GetLimitOffset(0, 4);
        REQUIRE(recordList->size() == 4);

        uint32_t cnt = 1;
        for (const auto &w : *recordList) {

            REQUIRE(w.ID == cnt++);

            REQUIRE(w.primaryName == primaryNameTest);
            REQUIRE(w.alternativeName == alternativeNameTest);
            REQUIRE(w.numbers[0].numberType == contactNumberTypeTest);
            REQUIRE(w.address == addressTest);
            REQUIRE(w.note == noteTest);
            REQUIRE(w.mail == mailTest);
            REQUIRE(w.assetPath == assetPath);
            REQUIRE(w.speeddial == speeddialTest);
        }
    }

    SECTION("Get records by limit offset by PrimaryName field")
    {
        auto recordList =
            contRecInterface.GetLimitOffsetByField(0, 4, ContactRecordField::PrimaryName, primaryNameTest);
        REQUIRE(recordList->size() == 4);

        uint32_t cnt = 1;
        for (const auto &w : *recordList) {

            REQUIRE(w.ID == cnt++);

            REQUIRE(w.primaryName == primaryNameTest);
            REQUIRE(w.alternativeName == alternativeNameTest);
            REQUIRE(w.numbers[0].numberType == contactNumberTypeTest);
            REQUIRE(w.address == addressTest);
            REQUIRE(w.note == noteTest);
            REQUIRE(w.mail == mailTest);
            REQUIRE(w.assetPath == assetPath);
            REQUIRE(w.speeddial == speeddialTest);
        }
    }
}

TEST_CASE("Test contact name formatting")
{
    const std::string primaryNameTest             = "PrimaryNameTest";
    const std::string alternativeNameTest         = "AlternativeNameTest";
    const std::string numberUserTest              = "600123456";
    const std::string numberE164Test              = "+48600123456";
    const std::string numberFormattedTest         = "600 123 456";
    const ContactNumberType contactNumberTypeTest = ContactNumberType::CELL;

    ContactRecord testRecord;

    testRecord.primaryName     = primaryNameTest;
    testRecord.alternativeName = alternativeNameTest;
    testRecord.numbers         = std::vector<ContactRecord::Number>({
        ContactRecord::Number(numberUserTest, numberE164Test, contactNumberTypeTest),
    });

    std::stringstream primaryAlternativeNameFormat;
    primaryAlternativeNameFormat << primaryNameTest << ' ' << alternativeNameTest;

    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == primaryAlternativeNameFormat.str());
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == primaryAlternativeNameFormat.str());
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::NotUseNumber) ==
            primaryAlternativeNameFormat.str());
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) == primaryAlternativeNameFormat.str());

    testRecord.primaryName = "";

    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == alternativeNameTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == alternativeNameTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::NotUseNumber) == alternativeNameTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) == alternativeNameTest);

    testRecord.primaryName     = primaryNameTest;
    testRecord.alternativeName = "";

    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == primaryNameTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == primaryNameTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::NotUseNumber) == primaryNameTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) == primaryNameTest);

    testRecord.primaryName = "";

    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == numberFormattedTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == numberFormattedTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::NotUseNumber) == "");
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) ==
            utils::translate("app_phonebook_contact_no_name"));

    testRecord.numbers.clear();
    testRecord.numbers = std::vector<ContactRecord::Number>({
        ContactRecord::Number(),
        ContactRecord::Number(numberUserTest, numberE164Test, contactNumberTypeTest),
    });

    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == numberFormattedTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == numberFormattedTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::NotUseNumber) == "");
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) ==
            utils::translate("app_phonebook_contact_no_name"));

    testRecord.numbers.clear();

    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == "");
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) ==
            utils::translate("app_phonebook_contact_no_name"));
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::NotUseNumber) == "");
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) ==
            utils::translate("app_phonebook_contact_no_name"));
}

TEST_CASE("Test converting contact data to string")
{
    const std::string primaryNameTest             = "PrimaryNameTest";
    const std::string alternativeNameTest         = "AlternativeNameTest";
    const std::string numberUserTest              = "600123456";
    const std::string numberE164Test              = "+48600123456";
    const std::string numberFormattedTest         = "600 123 456";
    const std::string mailTest                    = "TestMail";
    const std::string addressTest                 = "6 Czeczota St.\n02600 Warsaw";
    const std::string noteTest                    = "TestNote";
    const ContactNumberType contactNumberTypeTest = ContactNumberType::CELL;

    ContactRecord testRecord;

    testRecord.primaryName     = primaryNameTest;
    testRecord.alternativeName = alternativeNameTest;
    testRecord.numbers         = std::vector<ContactRecord::Number>({
        ContactRecord::Number(numberUserTest, numberE164Test, contactNumberTypeTest),
    });
    testRecord.mail            = mailTest;
    testRecord.address         = addressTest;
    testRecord.note            = noteTest;

    const std::string contactDataAll = primaryNameTest + " " + alternativeNameTest + "\n" + numberFormattedTest + "\n" +
                                       mailTest + "\n" + addressTest + "\n" + noteTest;
    const std::string contactDataTwoFieldsFromTheMiddle = numberFormattedTest + "\n" + mailTest + "\n";
    const std::string contactDataSingleField            = numberFormattedTest + "\n";

    auto contactDataStr = testRecord.getAsString();
    REQUIRE(contactDataStr == contactDataAll);

    testRecord.primaryName.clear();
    testRecord.alternativeName.clear();
    testRecord.address.clear();
    testRecord.note.clear();
    contactDataStr = testRecord.getAsString();
    REQUIRE(contactDataStr == contactDataTwoFieldsFromTheMiddle);

    testRecord.mail.clear();
    contactDataStr = testRecord.getAsString();
    REQUIRE(contactDataStr == contactDataSingleField);
}

TEST_CASE("Contact record numbers update")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    auto records = ContactRecordInterface(&contactsDb.get());

    ContactRecord testRecord, otherRecord;
    std::array<std::string, 4> numbers   = {{{"600100100"}, {"600100200"}, {"600100300"}, {"600100400"}}};
    std::array<std::string, 4> numbersPL = {
        {{"+48600100100"}, {"+48600100200"}, {"+48600100300"}, {"+48600100400"}}}; // Poland country code
    std::array<std::string, 4> numbersFR = {
        {{"+33600100100"}, {"+33600100200"}, {"+33600100300"}, {"+33600100400"}}}; // France country code
    std::array<std::string, 2> unUsedNumbers = {{{"612130140"}, {"612130141"}}};

    testRecord.primaryName     = "number";
    testRecord.alternativeName = "test";
    testRecord.numbers         = std::vector<ContactRecord::Number>(
        {ContactRecord::Number(numbers[0], std::string("")), ContactRecord::Number(numbers[1], std::string(""))});
    REQUIRE(records.Add(testRecord));

    otherRecord.primaryName     = "other";
    otherRecord.alternativeName = "record";
    otherRecord.numbers         = std::vector<ContactRecord::Number>(
        {ContactRecord::Number(numbers[2], std::string("")), ContactRecord::Number(numbers[3], std::string(""))});
    REQUIRE(records.Add(otherRecord));

    SECTION("No number update")
    {
        auto newRecord = records.GetByID(1);
        REQUIRE(newRecord.numbers.size() == 2);
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactsDb.get().number.count() == 4);

        auto validatationRecord = records.GetByID(1);
        REQUIRE(validatationRecord.numbers.size() == 2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[0]);
        REQUIRE(validatationRecord.numbers[1].number.getEntered() == numbers[1]);

        validatationRecord = records.GetByID(2);
        REQUIRE(validatationRecord.numbers.size() == 2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[2]);
        REQUIRE(validatationRecord.numbers[1].number.getEntered() == numbers[3]);
    }

    SECTION("Remove and add number")
    {
        auto newRecord = records.GetByID(1);
        REQUIRE(newRecord.numbers.size() == 2);

        newRecord.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(numbers[1], std::string(""))});
        REQUIRE(records.Update(newRecord));

        REQUIRE(contactsDb.get().number.count() == 4);

        auto validatationRecord = records.GetByID(1);
        REQUIRE(validatationRecord.numbers.size() == 1);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[1]);

        validatationRecord = records.GetByID(2);
        REQUIRE(validatationRecord.numbers.size() == 2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[2]);
        REQUIRE(validatationRecord.numbers[1].number.getEntered() == numbers[3]);

        newRecord.numbers = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(numbers[0], std::string("")), ContactRecord::Number(numbers[1], std::string(""))});
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactsDb.get().number.count() == 4);

        validatationRecord = records.GetByID(1);
        REQUIRE(validatationRecord.numbers.size() == 2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[0]);
        REQUIRE(validatationRecord.numbers[1].number.getEntered() == numbers[1]);

        validatationRecord = records.GetByID(2);
        REQUIRE(validatationRecord.numbers.size() == 2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[2]);
        REQUIRE(validatationRecord.numbers[1].number.getEntered() == numbers[3]);
    }

    SECTION("Change numbers positions")
    {
        auto newRecord = records.GetByID(1);
        REQUIRE(newRecord.numbers.size() == 2);
        REQUIRE(newRecord.numbers[0].number.getEntered() == numbers[0]);
        REQUIRE(newRecord.numbers[1].number.getEntered() == numbers[1]);

        newRecord.numbers = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(numbers[1], std::string("")), ContactRecord::Number(numbers[0], std::string(""))});
        REQUIRE(records.Update(newRecord));

        auto validatationRecord = records.GetByID(1);
        REQUIRE(contactsDb.get().number.count() == 4);
        REQUIRE(validatationRecord.numbers.size() == 2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[1]);
        REQUIRE(validatationRecord.numbers[1].number.getEntered() == numbers[0]);
    }

    SECTION("Single number update")
    {
        auto newRecord = records.GetByID(1);
        REQUIRE(newRecord.numbers.size() == 2);

        newRecord.numbers = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(numbers[2], std::string("")), ContactRecord::Number(numbers[1], std::string(""))});
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactsDb.get().number.count() == 4);

        auto validatationRecord = records.GetByID(1);
        REQUIRE(validatationRecord.numbers.size() == 2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[2]);
        REQUIRE(validatationRecord.numbers[1].number.getEntered() == numbers[1]);

        validatationRecord = records.GetByID(2);
        REQUIRE(validatationRecord.numbers.size() == 1);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[3]);
    }

    SECTION("Single number update - adding country code")
    {
        auto newRecord = records.GetByID(1);
        REQUIRE(newRecord.numbers.size() == 2);

        newRecord.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(numbersPL[0], std::string("")),
                                                                ContactRecord::Number(numbersFR[1], std::string(""))});
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactsDb.get().number.count() == 4);

        auto validationRecord = records.GetByID(1);
        REQUIRE(validationRecord.numbers.size() == 2);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbersPL[0]);
        REQUIRE(validationRecord.numbers[1].number.getEntered() == numbersFR[1]);

        validationRecord = records.GetByID(2);
        REQUIRE(validationRecord.numbers.size() == 2);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbers[2]);

        // There is no new temporary contact
        validationRecord = records.GetByIdWithTemporary(3);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());
    }

    SECTION("Single number update - removing country code")
    {
        auto newRecord = records.GetByID(1);
        REQUIRE(newRecord.numbers.size() == 2);

        // add country code
        newRecord.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(numbersPL[0], std::string("")),
                                                                ContactRecord::Number(numbersFR[1], std::string(""))});
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactsDb.get().number.count() == 4);

        // deleting country code
        newRecord.numbers = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(numbers[0], std::string("")), ContactRecord::Number(numbers[1], std::string(""))});
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactsDb.get().number.count() == 4);

        auto validationRecord = records.GetByID(1);
        REQUIRE(validationRecord.numbers.size() == 2);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbers[0]);
        REQUIRE(validationRecord.numbers[1].number.getEntered() == numbers[1]);

        validationRecord = records.GetByID(2);
        REQUIRE(validationRecord.numbers.size() == 2);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbers[2]);

        // There is no new temporary contact
        validationRecord = records.GetByIdWithTemporary(3);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());
    }

    SECTION("Single number update - changing country code")
    {
        auto newRecord = records.GetByID(1);
        REQUIRE(newRecord.numbers.size() == 2);

        // add country code (PL)
        newRecord.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(numbersPL[0], std::string("")),
                                                                ContactRecord::Number(numbersPL[1], std::string(""))});
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactsDb.get().number.count() == 4);

        // changing country code (to FR)
        newRecord.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(numbersFR[0], std::string("")),
                                                                ContactRecord::Number(numbersFR[1], std::string(""))});
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactsDb.get().number.count() == 6);
        REQUIRE(contactsDb.get().number.getById(1).contactID != 1); // old numbers do not belong to any contact
        REQUIRE(contactsDb.get().number.getById(1).contactID != 2);
        REQUIRE(contactsDb.get().number.getById(2).contactID != 1);
        REQUIRE(contactsDb.get().number.getById(2).contactID != 2);

        auto validationRecord = records.GetByID(1);
        REQUIRE(validationRecord.numbers.size() == 2);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbersFR[0]);
        REQUIRE(validationRecord.numbers[1].number.getEntered() == numbersFR[1]);

        validationRecord = records.GetByID(2);
        REQUIRE(validationRecord.numbers.size() == 2);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbers[2]);

        // A temporary contact for number ID 1 (which was previously assigned to recordID=1) has been created.
        validationRecord = records.GetByIdWithTemporary(3);
        REQUIRE(validationRecord.ID != DB_ID_NONE);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbersPL[0]);

        // A temporary contact for numberID 2 (which was previously assigned to recordID=2 for a moment) has been
        // created.
        validationRecord = records.GetByIdWithTemporary(4);
        REQUIRE(validationRecord.ID != DB_ID_NONE);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbersPL[1]);
    }

    SECTION("Change both numbers")
    {
        auto newRecord = records.GetByID(1);
        REQUIRE(newRecord.numbers.size() == 2);

        newRecord.numbers = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(numbers[2], std::string("")), ContactRecord::Number(numbers[3], std::string(""))});
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactsDb.get().number.count() == 4);

        auto validationRecord = records.GetByIdWithTemporary(1);
        REQUIRE(validationRecord.ID != DB_ID_NONE);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbers[2]);
        REQUIRE(validationRecord.numbers[1].number.getEntered() == numbers[3]);

        validationRecord = records.GetByIdWithTemporary(2);
        REQUIRE(validationRecord.ID != DB_ID_NONE);
        // numbers[3] was previously assigned to it, but it's re-assigned to recordID=2 above.
        REQUIRE(validationRecord.numbers.empty());

        // A temporary contact for number[0] (which was previously assigned to recordID=1) has been created.
        validationRecord = records.GetByIdWithTemporary(3);
        REQUIRE(validationRecord.ID != DB_ID_NONE);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbers[0]);

        // A temporary contact for number[1] (which was previously assigned to recordID=1) has been created.
        validationRecord = records.GetByIdWithTemporary(4);
        REQUIRE(validationRecord.ID != DB_ID_NONE);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbers[1]);
    }

    Database::deinitialize();
}

TEST_CASE("Contacts list merge")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    // Preparation of DB initial state
    auto records                                                          = ContactRecordInterface(&contactsDb.get());
    std::array<std::pair<std::string, std::string>, 3> rawContactsInitial = {
        {{"600100100", "test1"}, {"600100200", "test2"}, {"600100300", "test3"}}};
    for (auto &rawContact : rawContactsInitial) {
        ContactRecord record;
        record.primaryName = rawContact.second;
        record.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(rawContact.first, std::string(""))});
        REQUIRE(records.Add(record));
    }

    SECTION("Merge contacts without overlaps")
    {
        std::array<std::pair<std::string, std::string>, 3> rawContactsToAdd = {
            {{"600100400", "test4"}, {"600100500", "test5"}, {"600100600", "test6"}}};

        // Prepare contacts list to merge
        std::vector<ContactRecord> contacts;
        for (auto &rawContact : rawContactsToAdd) {
            ContactRecord record;
            record.primaryName = rawContact.second;
            record.numbers =
                std::vector<ContactRecord::Number>({ContactRecord::Number(rawContact.first, std::string(""))});
            contacts.push_back(record);
        }
        REQUIRE(!records.MergeContactsList(contacts).empty());

        // Validate if non-overlapping were appended to DB
        REQUIRE(records.GetCount() == (rawContactsInitial.size() + rawContactsToAdd.size()));

        auto validatationRecord = records.GetByID(1);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsInitial[0].first);
        REQUIRE(validatationRecord.primaryName == rawContactsInitial[0].second);
        validatationRecord = records.GetByID(2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsInitial[1].first);
        REQUIRE(validatationRecord.primaryName == rawContactsInitial[1].second);
        validatationRecord = records.GetByID(3);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsInitial[2].first);
        REQUIRE(validatationRecord.primaryName == rawContactsInitial[2].second);
        validatationRecord = records.GetByID(4);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsToAdd[0].first);
        REQUIRE(validatationRecord.primaryName == rawContactsToAdd[0].second);
        validatationRecord = records.GetByID(5);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsToAdd[1].first);
        REQUIRE(validatationRecord.primaryName == rawContactsToAdd[1].second);
        validatationRecord = records.GetByID(6);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsToAdd[2].first);
        REQUIRE(validatationRecord.primaryName == rawContactsToAdd[2].second);
    }

    SECTION("Merge contacts with numbers overlaps")
    {
        REQUIRE(records.GetCount() == rawContactsInitial.size());

        std::array<std::pair<std::string, std::string>, 3> rawContactsOverlapping = {
            {{rawContactsInitial[1].first, "test7"}, {"600100800", "test8"}, {rawContactsInitial[0].first, "test9"}}};
        constexpr auto numberOfNewContacts = 1;

        // Prepare contacts list to merge
        std::vector<ContactRecord> contacts;
        for (auto &rawContact : rawContactsOverlapping) {
            ContactRecord record;
            record.primaryName = rawContact.second;
            record.numbers =
                std::vector<ContactRecord::Number>({ContactRecord::Number(rawContact.first, std::string(""))});
            contacts.push_back(record);
        }
        REQUIRE(!records.MergeContactsList(contacts).empty());

        REQUIRE(records.GetCount() == (rawContactsInitial.size() + numberOfNewContacts));

        // Overlapping contacts replaced with same ID
        auto validatationRecord = records.GetByID(1);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsInitial[0].first);
        REQUIRE(validatationRecord.primaryName == rawContactsOverlapping[2].second);
        validatationRecord = records.GetByID(2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsInitial[1].first);
        REQUIRE(validatationRecord.primaryName == rawContactsOverlapping[0].second);
        // Non-overlapping contact left untouched
        validatationRecord = records.GetByID(3);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsInitial[2].first);
        REQUIRE(validatationRecord.primaryName == rawContactsInitial[2].second);
        // Non-overlapping new contact added
        validatationRecord = records.GetByID(4);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContactsOverlapping[1].first);
        REQUIRE(validatationRecord.primaryName == rawContactsOverlapping[1].second);
    }
}

TEST_CASE("Contacts list merge - advanced cases")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    // Preparation of DB initial state
    auto records = ContactRecordInterface(&contactsDb.get());
    // 3 numbers in single contact
    std::array<std::string, 3> numbers = {"600100100", "600100200", "600100300"};
    ContactRecord record;
    record.primaryName = "test";
    for (auto &number : numbers) {
        record.numbers.push_back({ContactRecord::Number(number, std::string(""))});
    }
    REQUIRE(records.Add(record));

    SECTION("Compared number is secondary number")
    {
        std::pair<std::string, std::string> rawContact = {"600100200", "test2"};
        std::vector<ContactRecord> contacts;

        // Prepare contacts list to merge
        ContactRecord record;
        record.primaryName = rawContact.second;
        record.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(rawContact.first, std::string(""))});
        contacts.push_back(record);
        REQUIRE(!records.MergeContactsList(contacts).empty());

        REQUIRE(records.GetCount() == 1);

        // First contact replaced
        auto validatationRecord = records.GetByID(1);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == rawContact.first);
        REQUIRE(validatationRecord.primaryName == rawContact.second);
    }
}

TEST_CASE("Contacts list duplicates search")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    // Preparation of DB initial state
    auto records                                                          = ContactRecordInterface(&contactsDb.get());
    std::array<std::pair<std::string, std::string>, 3> rawContactsInitial = {
        {{"600100100", "test1"}, {"600100200", "test2"}, {"600100300", "test3"}}};
    for (auto &rawContact : rawContactsInitial) {
        ContactRecord record;
        record.primaryName = rawContact.second;
        record.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(rawContact.first, std::string(""))});
        REQUIRE(records.Add(record));
    }

    // Prepare contacts list to compare with DB
    std::pair<std::string, std::string> uniqueContact                     = {"600100500", "test5"};
    std::array<std::pair<std::string, std::string>, 3> rawContactsToCheck = {
        {rawContactsInitial[2], uniqueContact, rawContactsInitial[0]}};
    constexpr auto numOfUniqueContacts     = 1;
    constexpr auto numOfDuplicatedContacts = 2;

    std::vector<ContactRecord> contacts;
    for (auto &rawContact : rawContactsToCheck) {
        ContactRecord record;
        record.primaryName = rawContact.second;
        record.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(rawContact.first, std::string(""))});
        contacts.push_back(record);
    }
    auto results    = records.CheckContactsListDuplicates(contacts);
    auto unique     = results.first;
    auto duplicates = results.second;

    REQUIRE(unique.size() == numOfUniqueContacts);

    REQUIRE(unique[0].numbers[0].number.getEntered() == uniqueContact.first);
    REQUIRE(unique[0].primaryName == uniqueContact.second);

    REQUIRE(duplicates.size() == numOfDuplicatedContacts);

    REQUIRE(duplicates[0].numbers[0].number.getEntered() == rawContactsToCheck[0].first);
    REQUIRE(duplicates[0].primaryName == rawContactsToCheck[0].second);

    REQUIRE(duplicates[1].numbers[0].number.getEntered() == rawContactsToCheck[2].first);
    REQUIRE(duplicates[1].primaryName == rawContactsToCheck[2].second);
}

TEST_CASE("Check if new contact record can be recognised as a duplicate in DB")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    // Preparation of DB initial state
    auto records = ContactRecordInterface(&contactsDb.get());
    ContactRecord testContactRecord;

    testContactRecord.primaryName     = "PrimaryNameTest";
    testContactRecord.alternativeName = "AlternativeNameTest";
    testContactRecord.numbers         = std::vector<ContactRecord::Number>({
        ContactRecord::Number("600123456", "+48600123456", ContactNumberType::HOME),
    });

    REQUIRE(records.Add(testContactRecord));

    SECTION("No duplicate")
    {
        ContactRecord noDuplicateContactRecord;
        noDuplicateContactRecord.primaryName     = "PrimaryNameTest2";
        noDuplicateContactRecord.alternativeName = "AlternativeNameTest2";
        noDuplicateContactRecord.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number("600123451", "+48600123451", ContactNumberType::HOME),
        });

        REQUIRE(records.verifyDuplicate(noDuplicateContactRecord).empty());
    }

    SECTION("Duplicate")
    {
        ContactRecord duplicateContactRecord;

        duplicateContactRecord.primaryName     = "PrimaryNameDuplicate";
        duplicateContactRecord.alternativeName = "AlternativeNameDuplicate";
        duplicateContactRecord.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number("600123456", "+48600123456", ContactNumberType::HOME),
        });

        REQUIRE(!records.verifyDuplicate(duplicateContactRecord).empty());
    }
}

TEST_CASE("Check if new contact record exists in DB as a temporary contact")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    // Preparation of DB initial state
    auto records = ContactRecordInterface(&contactsDb.get());
    ContactRecord testContactRecord;

    testContactRecord.primaryName     = "PrimaryNameTest";
    testContactRecord.alternativeName = "AlternativeNameTest";
    testContactRecord.numbers         = std::vector<ContactRecord::Number>({
        ContactRecord::Number("600123456", "+48600123456", ContactNumberType::HOME),
    });

    REQUIRE(records.Add(testContactRecord));

    SECTION("No temporary contact exists")
    {
        ContactRecord anotherTestContactRecord;
        anotherTestContactRecord.primaryName     = "PrimaryNameTest2";
        anotherTestContactRecord.alternativeName = "AlternativeNameTest2";
        anotherTestContactRecord.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number("600123451", "+48600123451", ContactNumberType::HOME),
        });

        REQUIRE(records.verifyTemporary(anotherTestContactRecord) == false);
    }

    SECTION("Temporary contact exists")
    {
        ContactRecord temporaryContactRecord;
        temporaryContactRecord.numbers = std::vector<ContactRecord::Number>({
            ContactRecord::Number("600123452", "+48600123452", ContactNumberType::HOME),
        });
        temporaryContactRecord.addToGroup(contactsDb.get().groups.temporaryId());

        REQUIRE(records.Add(temporaryContactRecord));

        ContactRecord noTemporaryContactRecord;
        noTemporaryContactRecord.primaryName     = "PrimaryNameNoTemporary";
        noTemporaryContactRecord.alternativeName = "AlternativeNameNoTemporary";
        noTemporaryContactRecord.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number("600123452", "+48600123452", ContactNumberType::HOME),
        });

        REQUIRE(records.verifyTemporary(noTemporaryContactRecord) == true);
    }
}

TEST_CASE("Check replacement of number in place in db when only different is having of country code")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    // Preparation of DB initial state
    auto records = ContactRecordInterface(&contactsDb.get());

    std::array<std::string, 2> numbers   = {{{"500600100"}, {"500600200"}}};
    std::array<std::string, 2> numbersPL = {{{"+48500600100"}, {"+48500600200"}}}; // Poland country code
    std::array<std::string, 2> numbersFR = {{{"+33500600100"}, {"+33500600200"}}}; // France country code

    SECTION("Temporary contact exists but new one with same number have country code")
    {
        // Number of record in number table should stay the same
        constexpr int32_t NUMBER_IN_RECORD_DURING_REPLACEMENT = 1;

        // Adding of temporary contact without country code
        ContactRecord temporaryContactRecord;
        temporaryContactRecord.numbers = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numbers[0], std::string(""), ContactNumberType::HOME),
        });
        temporaryContactRecord.addToGroup(contactsDb.get().groups.temporaryId());

        REQUIRE(records.Add(temporaryContactRecord));
        REQUIRE(contactsDb.get().number.count() == NUMBER_IN_RECORD_DURING_REPLACEMENT);

        auto validationRecord = records.GetByID(1); // There aren't new normal contact
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        validationRecord = records.GetByIdWithTemporary(1); // There are new temporary contact
        REQUIRE(validationRecord.numbers.size() == 1);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbers[0]);

        // Adding of normal contact witch same number than previous temporary but with country code
        ContactRecord noTemporaryContactRecord;
        noTemporaryContactRecord.primaryName     = "PrimaryNameNoTemporary";
        noTemporaryContactRecord.alternativeName = "AlternativeNameNoTemporary";
        noTemporaryContactRecord.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numbersPL[0], std::string(""), ContactNumberType::HOME),
        });

        REQUIRE(records.verifyTemporary(noTemporaryContactRecord) == true);

        REQUIRE(records.Add(noTemporaryContactRecord));
        REQUIRE(contactsDb.get().number.count() == NUMBER_IN_RECORD_DURING_REPLACEMENT); // That was replacement in
                                                                                         // place

        validationRecord = records.GetByID(1);
        REQUIRE(validationRecord.numbers.size() == 0); // Now first contact is temporary
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        validationRecord = records.GetByID(2);
        REQUIRE(validationRecord.numbers.size() == 1);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbersPL[0]);
    }

    SECTION("Adding contact without country code after deleting one with same number but with country code")
    {
        // Scenario from MOS-864:
        // After deleting contact with prefix: e.g. +48 512 345 678, there was an issue when
        // new contact with similar number but without country code "512 345 678" was added.
        // Then new contact had number with prefix: "+48 512 345 678" instead of provided number by user

        // Number of record in number table should stay the same
        constexpr int32_t NUMBER_IN_RECORD_DURING_REPLACEMENT = 1;
        constexpr int32_t FIRST_CONTACT_ID                    = 1;

        // Adding of normal contact with country code
        ContactRecord noTemporaryContactRecord;
        noTemporaryContactRecord.primaryName     = "PrimaryNameNoTemporary";
        noTemporaryContactRecord.alternativeName = "AlternativeNameNoTemporary";
        noTemporaryContactRecord.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numbersPL[0], std::string(""), ContactNumberType::HOME),
        });

        REQUIRE(records.Add(noTemporaryContactRecord));
        REQUIRE(contactsDb.get().number.count() == NUMBER_IN_RECORD_DURING_REPLACEMENT);

        auto validationRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.numbers.size() == 1);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbersPL[0]); // with country code

        // Remove contact
        REQUIRE(records.RemoveByID(FIRST_CONTACT_ID));

        validationRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.numbers.size() == 0); // Now first contact is temporary
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        // Add new contact with same number as previously deleted but without country code
        ContactRecord NewNoTemporaryContactRecord;
        NewNoTemporaryContactRecord.primaryName     = "PrimaryNameNoTemporaryOther";
        NewNoTemporaryContactRecord.alternativeName = "AlternativeNameNoTemporaryOther";
        NewNoTemporaryContactRecord.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numbers[0], std::string(""), ContactNumberType::HOME),
        });

        REQUIRE(records.Add(NewNoTemporaryContactRecord));
        REQUIRE(contactsDb.get().number.count() == NUMBER_IN_RECORD_DURING_REPLACEMENT);

        validationRecord = records.GetByID(FIRST_CONTACT_ID + 1); // New contact
        REQUIRE(validationRecord.numbers.size() == 1);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbers[0]); // without country code
    }

    SECTION("Adding contact with country code after deleting one with same number but with different country code")
    {
        // Number of record in number table should NOT stay the same - no replacement in place
        constexpr int32_t FIRST_CONTACT_ID      = 1;
        constexpr int32_t SECOND_NEW_CONTACT_ID = 2;

        // Adding of normal contact with country code
        ContactRecord noTemporaryContactRecordPL;
        noTemporaryContactRecordPL.primaryName     = "PrimaryNameNoTemporary";
        noTemporaryContactRecordPL.alternativeName = "AlternativeNameNoTemporary";
        noTemporaryContactRecordPL.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numbersPL[0], std::string(""), ContactNumberType::HOME),
        });

        REQUIRE(records.Add(noTemporaryContactRecordPL));
        REQUIRE(contactsDb.get().number.count() == 1);

        auto validationRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.numbers.size() == 1);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbersPL[0]); // with country code

        // Remove contact
        REQUIRE(records.RemoveByID(FIRST_CONTACT_ID));

        validationRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.numbers.size() == 0); // Now first contact is temporary
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        // Add new contact with same number as previously deleted but without country code
        ContactRecord NewNoTemporaryContactRecordFR;
        NewNoTemporaryContactRecordFR.primaryName     = "PrimaryNameNoTemporaryOther";
        NewNoTemporaryContactRecordFR.alternativeName = "AlternativeNameNoTemporaryOther";
        NewNoTemporaryContactRecordFR.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numbersFR[0], std::string(""), ContactNumberType::HOME),
        });

        REQUIRE(records.Add(NewNoTemporaryContactRecordFR));
        REQUIRE(contactsDb.get().number.count() == 2); // New number record - there are record for PL and FR number

        validationRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.numbers.size() == 0); // Now first contact is temporary
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        validationRecord = records.GetByID(SECOND_NEW_CONTACT_ID); // New contact
        REQUIRE(validationRecord.numbers.size() == 1);
        REQUIRE(validationRecord.numbers[0].number.getEntered() == numbersFR[0]); // without country code

        // Check number table
        REQUIRE(contactsDb.get().number.getById(FIRST_CONTACT_ID).numberUser == numbersPL[0]);
        REQUIRE(contactsDb.get().number.getById(SECOND_NEW_CONTACT_ID).numberUser == numbersFR[0]);
    }
}

TEST_CASE("Check if contact can have 2 similar number (exactly the same or just with/with no country code))")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};

    // Preparation of DB initial state
    auto records = ContactRecordInterface(&contactsDb.get());

    std::string number                 = "500600100";
    std::string numberPL               = "+48500600100"; // Poland country code
    constexpr int32_t FIRST_CONTACT_ID = 1;

    SECTION("Add a new contact with 2 exactly the same number (without country code)")
    {
        // Adding of normal contact with 2 exactly the same numbers
        ContactRecord noTempContactRecordExactlySameNumbers;
        noTempContactRecordExactlySameNumbers.primaryName     = "PrimaryNameNoTemporary";
        noTempContactRecordExactlySameNumbers.alternativeName = "AlternativeNameNoTemporary";
        noTempContactRecordExactlySameNumbers.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number(number, std::string(""), ContactNumberType::HOME),
            ContactRecord::Number(number, std::string(""), ContactNumberType::HOME),
        });

        REQUIRE(records.Add(noTempContactRecordExactlySameNumbers) ==
                false);                                // Try to add contact witch the same numbers
        REQUIRE(contactsDb.get().number.count() == 0); // There should be no contact

        // No new contact
        auto validationRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        // No new temporary contact
        validationRecord = records.GetByIdWithTemporary(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());
    }

    SECTION("Add a new contact with 2 exactly the same number (with country code)")
    {
        // Adding of normal contact with 2 exactly the same numbers
        ContactRecord noTempContactRecordExactlySameNumbers;
        noTempContactRecordExactlySameNumbers.primaryName     = "PrimaryNameNoTemporary";
        noTempContactRecordExactlySameNumbers.alternativeName = "AlternativeNameNoTemporary";
        noTempContactRecordExactlySameNumbers.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numberPL, std::string(""), ContactNumberType::HOME),
            ContactRecord::Number(numberPL, std::string(""), ContactNumberType::HOME),
        });

        REQUIRE(records.Add(noTempContactRecordExactlySameNumbers) ==
                false);                                // Try to add contact witch the same numbers
        REQUIRE(contactsDb.get().number.count() == 0); // There should be no contact

        // No new contact
        auto validationRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        // No new temporary contact
        validationRecord = records.GetByIdWithTemporary(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());
    }

    SECTION("Add a new contact with 2 same numbers but one with country code and one without")
    {
        // Adding of normal contact with 2 same numbers
        ContactRecord noTempContactRecordExactlySameNumbers;
        noTempContactRecordExactlySameNumbers.primaryName     = "PrimaryNameNoTemporary";
        noTempContactRecordExactlySameNumbers.alternativeName = "AlternativeNameNoTemporary";
        noTempContactRecordExactlySameNumbers.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number(number, std::string(""), ContactNumberType::HOME),
            ContactRecord::Number(numberPL, std::string(""), ContactNumberType::HOME),
        });

        REQUIRE(records.Add(noTempContactRecordExactlySameNumbers) ==
                false);                                // Try to add contact witch the same numbers
        REQUIRE(contactsDb.get().number.count() == 0); // There should be no contact

        // No new contact
        auto validationRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        // No new temporary contact
        validationRecord = records.GetByIdWithTemporary(FIRST_CONTACT_ID);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());
    }

    SECTION("Update contact to contact with 2 exactly same numbers (without country code)")
    {
        // Adding of normal contact with
        ContactRecord noTempContactRecordExactlySameNumbers;
        noTempContactRecordExactlySameNumbers.primaryName     = "PrimaryNameNoTemporary";
        noTempContactRecordExactlySameNumbers.alternativeName = "AlternativeNameNoTemporary";
        noTempContactRecordExactlySameNumbers.numbers         = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(number, std::string(""), ContactNumberType::HOME)});

        // Add normal valid contact and check it
        REQUIRE(records.Add(noTempContactRecordExactlySameNumbers) == true);
        REQUIRE(contactsDb.get().number.count() == 1); // There should be no contact
        auto normalContactRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(normalContactRecord.numbers.size() == 1);
        REQUIRE(normalContactRecord.numbers[0].number.getEntered() == number); // without country code

        // Update contact to contact with the same numers
        auto recordToUpdate    = records.GetByID(FIRST_CONTACT_ID);
        recordToUpdate.numbers = std::vector<ContactRecord::Number>({
            ContactRecord::Number(number, std::string(""), ContactNumberType::HOME),
            ContactRecord::Number(number, std::string(""), ContactNumberType::HOME),
        });
        REQUIRE(records.Update(recordToUpdate) == false);

        // Check contact record after Update
        auto recordAfterUpdate = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(recordAfterUpdate.numbers.size() == 1);
        REQUIRE(recordAfterUpdate.numbers[0].number.getEntered() == number); // without country code

        // No new contact
        auto validationRecord = records.GetByID(FIRST_CONTACT_ID + 1);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        // No new temporary contact
        validationRecord = records.GetByIdWithTemporary(FIRST_CONTACT_ID + 1);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());
    }

    SECTION("Update contact to contact with 2 exactly same numbers (with country code)")
    {
        // Adding of normal contact
        ContactRecord noTempContactRecordExactlySameNumbers;
        noTempContactRecordExactlySameNumbers.primaryName     = "PrimaryNameNoTemporary";
        noTempContactRecordExactlySameNumbers.alternativeName = "AlternativeNameNoTemporary";
        noTempContactRecordExactlySameNumbers.numbers         = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(numberPL, std::string(""), ContactNumberType::HOME)});

        // Add normal valid contact and check it
        REQUIRE(records.Add(noTempContactRecordExactlySameNumbers) == true);
        REQUIRE(contactsDb.get().number.count() == 1); // There should be no contact
        auto normalContactRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(normalContactRecord.numbers.size() == 1);
        REQUIRE(normalContactRecord.numbers[0].number.getEntered() == numberPL); // without country code

        // Update contact to contact with the same numbers
        auto recordToUpdate    = records.GetByID(FIRST_CONTACT_ID);
        recordToUpdate.numbers = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numberPL, std::string(""), ContactNumberType::HOME),
            ContactRecord::Number(numberPL, std::string(""), ContactNumberType::HOME),
        });
        REQUIRE(records.Update(recordToUpdate) == false);

        // Check contact record after Update
        auto recordAfterUpdate = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(recordAfterUpdate.numbers.size() == 1);
        REQUIRE(recordAfterUpdate.numbers[0].number.getEntered() == numberPL); // without country code

        // No new contact
        auto validationRecord = records.GetByID(FIRST_CONTACT_ID + 1);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        // No new temporary contact
        validationRecord = records.GetByIdWithTemporary(FIRST_CONTACT_ID + 1);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());
    }

    SECTION("Update contact to contact with 2 same numbers but one without country code and new with country code")
    {
        // Adding of normal contact
        ContactRecord noTempContactRecordExactlySameNumbers;
        noTempContactRecordExactlySameNumbers.primaryName     = "PrimaryNameNoTemporary";
        noTempContactRecordExactlySameNumbers.alternativeName = "AlternativeNameNoTemporary";
        noTempContactRecordExactlySameNumbers.numbers         = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(number, std::string(""), ContactNumberType::HOME)});

        // Add normal valid contact and check it
        REQUIRE(records.Add(noTempContactRecordExactlySameNumbers) == true);
        REQUIRE(contactsDb.get().number.count() == 1); // There should be no contact
        auto normalContactRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(normalContactRecord.numbers.size() == 1);
        REQUIRE(normalContactRecord.numbers[0].number.getEntered() == number); // without country code

        // Update contact to contact with the same numbers
        auto recordToUpdate    = records.GetByID(FIRST_CONTACT_ID);
        recordToUpdate.numbers = std::vector<ContactRecord::Number>({
            ContactRecord::Number(number, std::string(""), ContactNumberType::HOME),
            ContactRecord::Number(numberPL, std::string(""), ContactNumberType::HOME),
        });
        REQUIRE(records.Update(recordToUpdate) == false);

        // Check contact record after Update
        auto recordAfterUpdate = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(recordAfterUpdate.numbers.size() == 1);
        REQUIRE(recordAfterUpdate.numbers[0].number.getEntered() == number); // without country code

        // No new contact
        auto validationRecord = records.GetByID(FIRST_CONTACT_ID + 1);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        // No new temporary contact
        validationRecord = records.GetByIdWithTemporary(FIRST_CONTACT_ID + 1);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());
    }

    SECTION("Update contact to contact with 2 same numbers but one with country code and new without")
    {
        // Adding of normal contact
        ContactRecord noTempContactRecordExactlySameNumbers;
        noTempContactRecordExactlySameNumbers.primaryName     = "PrimaryNameNoTemporary";
        noTempContactRecordExactlySameNumbers.alternativeName = "AlternativeNameNoTemporary";
        noTempContactRecordExactlySameNumbers.numbers         = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(numberPL, std::string(""), ContactNumberType::HOME)});

        // Add normal valid contact and check it
        REQUIRE(records.Add(noTempContactRecordExactlySameNumbers) == true);
        REQUIRE(contactsDb.get().number.count() == 1); // There should be no contact
        auto normalContactRecord = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(normalContactRecord.numbers.size() == 1);
        REQUIRE(normalContactRecord.numbers[0].number.getEntered() == numberPL); // without country code

        // Update contact to contact with the same numbers
        auto recordToUpdate    = records.GetByID(FIRST_CONTACT_ID);
        recordToUpdate.numbers = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numberPL, std::string(""), ContactNumberType::HOME),
            ContactRecord::Number(number, std::string(""), ContactNumberType::HOME),
        });
        REQUIRE(records.Update(recordToUpdate) == false);

        // Check contact record after Update
        auto recordAfterUpdate = records.GetByID(FIRST_CONTACT_ID);
        REQUIRE(recordAfterUpdate.numbers.size() == 1);
        REQUIRE(recordAfterUpdate.numbers[0].number.getEntered() == numberPL); // without country code

        // No new contact
        auto validationRecord = records.GetByID(FIRST_CONTACT_ID + 1);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());

        // No new temporary contact
        validationRecord = records.GetByIdWithTemporary(FIRST_CONTACT_ID + 1);
        REQUIRE(validationRecord.ID == DB_ID_NONE);
        REQUIRE(validationRecord.numbers.empty());
    }
}
