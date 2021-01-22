// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Interface/ContactRecord.hpp"
#include <i18n/i18n.hpp>
#include "vfs.hpp"
#include <purefs/filesystem_paths.hpp>

TEST_CASE("Contact Record db tests")
{
    vfs.Init();
    Database::initialize();

    const auto contactsPath = purefs::dir::getUserDiskPath() / "contacts.db";
    std::filesystem::remove(contactsPath);

    ContactsDB contactDB(contactsPath.c_str());
    REQUIRE(contactDB.isInitialized());

    const char *primaryNameTest                   = "PrimaryNameTest";
    const char *alternativeNameTest               = "AlternativeNameTest";
    const char *numberUserTest                    = "600123456";
    const char *numberE164Test                    = "+48600123456";
    const char *addressTest                       = "6 Czeczota St.\n02600 Warsaw";
    const char *noteTest                          = "TestNote";
    const char *mailTest                          = "TestMail";
    const char *assetPath                         = "/Test/Path/To/Asset";
    const char *speeddialTest                     = "100";
    const ContactNumberType contactNumberTypeTest = ContactNumberType ::PAGER;

    ContactRecordInterface contRecInterface(&contactDB);

    ContactRecord recordIN;

    recordIN.primaryName     = primaryNameTest;
    recordIN.alternativeName = alternativeNameTest;
    recordIN.numbers         = std::vector<ContactRecord::Number>({
        ContactRecord::Number(numberUserTest, numberE164Test, contactNumberTypeTest),
    });
    recordIN.address         = addressTest;
    recordIN.note            = noteTest;
    recordIN.mail            = mailTest;
    recordIN.assetPath       = assetPath;
    recordIN.speeddial       = speeddialTest;

    REQUIRE(contRecInterface.Add(recordIN));
    REQUIRE(contRecInterface.Add(recordIN));
    REQUIRE(contRecInterface.Add(recordIN));
    REQUIRE(contRecInterface.Add(recordIN));
    REQUIRE(contRecInterface.Add(recordIN));

    SECTION("Get record by ID")
    {
        auto recordOUT = contRecInterface.GetByID(1);

        REQUIRE(recordOUT.ID == 1);

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
            REQUIRE(w.numbers[0].number.getEntered() == numberUserTest);
            REQUIRE(w.numbers[0].number.getE164() == numberE164Test);
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
            REQUIRE(w.numbers[0].number.getEntered() == numberUserTest);
            REQUIRE(w.numbers[0].number.getE164() == numberE164Test);
            REQUIRE(w.numbers[0].numberType == contactNumberTypeTest);
            REQUIRE(w.address == addressTest);
            REQUIRE(w.note == noteTest);
            REQUIRE(w.mail == mailTest);
            REQUIRE(w.assetPath == assetPath);
            REQUIRE(w.speeddial == speeddialTest);
        }
    }

    SECTION("Get records by limit offset by NumberE164 filed")
    {
        auto recordList = contRecInterface.GetLimitOffsetByField(0, 2, ContactRecordField::NumberE164, numberE164Test);
        REQUIRE(recordList->size() == 2);

        uint32_t cnt = 1;
        for (const auto &w : *recordList) {

            REQUIRE(w.ID == cnt++);

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
    }

    Database::deinitialize();
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
            utils::localize.get("app_phonebook_contact_no_name"));

    testRecord.numbers.clear();
    testRecord.numbers = std::vector<ContactRecord::Number>({
        ContactRecord::Number(),
        ContactRecord::Number(numberUserTest, numberE164Test, contactNumberTypeTest),
    });

    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == numberFormattedTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == numberFormattedTest);
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::NotUseNumber) == "");
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) ==
            utils::localize.get("app_phonebook_contact_no_name"));

    testRecord.numbers.clear();

    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == "");
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) ==
            utils::localize.get("app_phonebook_contact_no_name"));
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::NotUseNumber) == "");
    REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) ==
            utils::localize.get("app_phonebook_contact_no_name"));
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
    vfs.Init();
    Database::initialize();
    const auto contactsPath = purefs::dir::getUserDiskPath() / "contacts.db";
    std::filesystem::remove(contactsPath);

    ContactsDB contactDB(contactsPath.c_str());
    REQUIRE(contactDB.isInitialized());

    auto records = ContactRecordInterface(&contactDB);

    ContactRecord testRecord, otherRecord;
    std::array<std::string, 4> numbers = {{{"600100100"}, {"600100200"}, {"600100300"}, {"600100400"}}};

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
        REQUIRE(contactDB.number.count() == 4);

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

        REQUIRE(contactDB.number.count() == 3);

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
        REQUIRE(contactDB.number.count() == 4);

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
        REQUIRE(contactDB.number.count() == 4);
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
        REQUIRE(contactDB.number.count() == 3);

        auto validatationRecord = records.GetByID(1);
        REQUIRE(validatationRecord.numbers.size() == 2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[2]);
        REQUIRE(validatationRecord.numbers[1].number.getEntered() == numbers[1]);

        validatationRecord = records.GetByID(2);
        REQUIRE(validatationRecord.numbers.size() == 1);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[3]);
    }

    SECTION("Change both numbers")
    {
        auto newRecord = records.GetByID(1);
        REQUIRE(newRecord.numbers.size() == 2);

        newRecord.numbers = std::vector<ContactRecord::Number>(
            {ContactRecord::Number(numbers[2], std::string("")), ContactRecord::Number(numbers[3], std::string(""))});
        REQUIRE(records.Update(newRecord));
        REQUIRE(contactDB.number.count() == 2);

        auto validatationRecord = records.GetByID(1);
        REQUIRE(validatationRecord.numbers.size() == 2);
        REQUIRE(validatationRecord.numbers[0].number.getEntered() == numbers[2]);
        REQUIRE(validatationRecord.numbers[1].number.getEntered() == numbers[3]);

        validatationRecord = records.GetByID(2);
        REQUIRE(validatationRecord.numbers.size() == 0);
    }

    Database::deinitialize();
}
