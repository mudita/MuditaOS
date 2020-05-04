#include "../Interface/ContactRecord.hpp"
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
    const char *addressLine1Test                  = "6 Czeczota St.";
    const char *addressLine2Test                  = "02600 Warsaw";
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
    recordIN.addressLine1    = addressLine1Test;
    recordIN.addressLine2    = addressLine2Test;
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
        REQUIRE(recordOUT.addressLine1 == addressLine1Test);
        REQUIRE(recordOUT.addressLine2 == addressLine2Test);
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
            REQUIRE(w.addressLine1 == addressLine1Test);
            REQUIRE(w.addressLine2 == addressLine2Test);
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
            REQUIRE(w.addressLine1 == addressLine1Test);
            REQUIRE(w.addressLine2 == addressLine2Test);
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
            REQUIRE(w.addressLine1 == addressLine1Test);
            REQUIRE(w.addressLine2 == addressLine2Test);
            REQUIRE(w.note == noteTest);
            REQUIRE(w.mail == mailTest);
            REQUIRE(w.assetPath == assetPath);
            REQUIRE(w.speeddial == speeddialTest);
        }
    }

    Database::Deinitialize();
}
