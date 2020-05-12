#include "../Interface/ContactRecord.hpp"
#include "i18/i18.hpp"
#include "vfs.hpp"

#include "catch.hpp"

TEST_CASE("Contact Record tests")
{
    Database::Initialize();

    vfs.remove(ContactsDB::GetDBName());

    auto contactDB = std::make_unique<ContactsDB>();
    REQUIRE(contactDB->IsInitialized());

    const char *primaryNameTest                   = "PrimaryNameTest";
    const char *alternativeNameTest               = "AlternativeNameTest";
    const char *numberUserTest                    = "111222333";
    const char *numberE164Test                    = "333222111";
    const char *addressTest                       = "6 Czeczota St.\n02600 Warsaw";
    const char *noteTest                          = "TestNote";
    const char *mailTest                          = "TestMail";
    const char *assetPath                         = "/Test/Path/To/Asset";
    const char *speeddialTest                     = "100";
    const ContactType contactTypeTest             = ContactType ::USER;
    const ContactNumberType contactNumberTypeTest = ContactNumberType ::PAGER;

    ContactRecordInterface contRecInterface(contactDB.get());

    ContactRecord recordIN;

    recordIN.primaryName     = primaryNameTest;
    recordIN.alternativeName = alternativeNameTest;
    recordIN.numbers         = std::vector<ContactRecord::Number>({
        ContactRecord::Number(numberUserTest, numberE164Test, contactNumberTypeTest),
    });
    recordIN.contactType     = contactTypeTest;
    recordIN.address         = addressTest;
    recordIN.note            = noteTest;
    recordIN.mail            = mailTest;
    recordIN.assetPath       = assetPath;
    recordIN.speeddial       = speeddialTest;

    REQUIRE(contRecInterface.Add(recordIN) == true);
    REQUIRE(contRecInterface.Add(recordIN) == true);
    REQUIRE(contRecInterface.Add(recordIN) == true);
    REQUIRE(contRecInterface.Add(recordIN) == true);
    REQUIRE(contRecInterface.Add(recordIN) == true);

    {
        auto recordOUT = contRecInterface.GetByID(1);

        REQUIRE(recordOUT.ID == 1);

        REQUIRE(recordOUT.primaryName == primaryNameTest);
        REQUIRE(recordOUT.alternativeName == alternativeNameTest);
        REQUIRE(recordOUT.numbers.size() == 1);
        REQUIRE(recordOUT.numbers[0].numberUser == numberUserTest);
        REQUIRE(recordOUT.numbers[0].numberE164 == numberE164Test);
        REQUIRE(recordOUT.numbers[0].numberType == contactNumberTypeTest);
        REQUIRE(recordOUT.contactType == contactTypeTest);
        REQUIRE(recordOUT.address == addressTest);
        REQUIRE(recordOUT.note == noteTest);
        REQUIRE(recordOUT.mail == mailTest);
        REQUIRE(recordOUT.assetPath == assetPath);
        REQUIRE(recordOUT.speeddial == speeddialTest);
    }

    REQUIRE(contRecInterface.RemoveByID(5) == true);

    {
        auto recordList = contRecInterface.GetLimitOffset(0, 4);
        REQUIRE(recordList->size() == 4);

        uint32_t cnt = 1;
        for (const auto w : *recordList) {

            REQUIRE(w.ID == cnt++);

            REQUIRE(w.primaryName == primaryNameTest);
            REQUIRE(w.alternativeName == alternativeNameTest);
            REQUIRE(w.numbers[0].numberUser == numberUserTest);
            REQUIRE(w.numbers[0].numberE164 == numberE164Test);
            REQUIRE(w.numbers[0].numberType == contactNumberTypeTest);
            REQUIRE(w.contactType == contactTypeTest);
            REQUIRE(w.address == addressTest);
            REQUIRE(w.note == noteTest);
            REQUIRE(w.mail == mailTest);
            REQUIRE(w.assetPath == assetPath);
            REQUIRE(w.speeddial == speeddialTest);
        }
    }

    {
        auto recordList =
            contRecInterface.GetLimitOffsetByField(0, 4, ContactRecordField::PrimaryName, primaryNameTest);
        REQUIRE(recordList->size() == 4);

        uint32_t cnt = 1;
        for (const auto w : *recordList) {

            REQUIRE(w.ID == cnt++);

            REQUIRE(w.primaryName == primaryNameTest);
            REQUIRE(w.alternativeName == alternativeNameTest);
            REQUIRE(w.numbers[0].numberUser == numberUserTest);
            REQUIRE(w.numbers[0].numberE164 == numberE164Test);
            REQUIRE(w.numbers[0].numberType == contactNumberTypeTest);
            REQUIRE(w.contactType == contactTypeTest);
            REQUIRE(w.address == addressTest);
            REQUIRE(w.note == noteTest);
            REQUIRE(w.mail == mailTest);
            REQUIRE(w.assetPath == assetPath);
            REQUIRE(w.speeddial == speeddialTest);
        }
    }

    {
        auto recordList = contRecInterface.GetLimitOffsetByField(0, 2, ContactRecordField::NumberE164, numberE164Test);
        REQUIRE(recordList->size() == 2);

        uint32_t cnt = 1;
        for (const auto w : *recordList) {

            REQUIRE(w.ID == cnt++);

            REQUIRE(w.primaryName == primaryNameTest);
            REQUIRE(w.alternativeName == alternativeNameTest);
            REQUIRE(w.numbers[0].numberUser == numberUserTest);
            REQUIRE(w.numbers[0].numberE164 == numberE164Test);
            REQUIRE(w.numbers[0].numberType == contactNumberTypeTest);
            REQUIRE(w.contactType == contactTypeTest);
            REQUIRE(w.address == addressTest);
            REQUIRE(w.note == noteTest);
            REQUIRE(w.mail == mailTest);
            REQUIRE(w.assetPath == assetPath);
            REQUIRE(w.speeddial == speeddialTest);
        }
    }

    {
        ContactRecord testRecord;

        testRecord.primaryName     = primaryNameTest;
        testRecord.alternativeName = alternativeNameTest;
        testRecord.numbers         = std::vector<ContactRecord::Number>({
            ContactRecord::Number(numberUserTest, numberE164Test, contactNumberTypeTest),
        });

        std::stringstream primaryAlternativeNameFormat;
        primaryAlternativeNameFormat << primaryNameTest << ' ' << alternativeNameTest;

        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) ==
                primaryAlternativeNameFormat.str());
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == primaryAlternativeNameFormat.str());
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) == primaryAlternativeNameFormat.str());

        testRecord.primaryName = "";

        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == alternativeNameTest);
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == alternativeNameTest);
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) == alternativeNameTest);

        testRecord.primaryName     = primaryNameTest;
        testRecord.alternativeName = "";

        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == primaryNameTest);
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == primaryNameTest);
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) == primaryNameTest);

        testRecord.primaryName = "";

        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == numberUserTest);
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == numberUserTest);
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) == utils::localize.get("app_phonebook_contact_no_name"));

        testRecord.numbers.clear();

        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Default) == "");
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::List) == utils::localize.get("app_phonebook_contact_no_name"));
        REQUIRE(testRecord.getFormattedName(ContactRecord::NameFormatType::Title) == utils::localize.get("app_phonebook_contact_no_name"));
    }

    Database::Deinitialize();
}
