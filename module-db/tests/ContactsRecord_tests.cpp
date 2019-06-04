
/*
 * @file ContactsRecord_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 03.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "vfs.hpp"

#include "catch.hpp"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <iostream>

#include "../Database/Database.hpp"
#include "../Interface/ContactRecord.hpp"
#include "../Databases/ContactsDB.hpp"


TEST_CASE("Contact Record tests")
{
    Database::Initialize();

    vfs.remove(ContactsDB::GetDBName());

    const char* primaryNameTest = "PrimaryNameTest";
    const char* alternativeNameTest = "AlternativeNameTest";
    const char* numberUserTest = "111222333";
    const char* numberE164Test = "333222111";
    const char* countryTest = "Poland";
    const char* cityTest = "Warsaw";
    const char* streetTest = "Czeczota";
    const char* numberTest = "7/9";
    const char* noteTest = "TestNote";
    const char* mailTest = "TestMail";
    const char* assetPath = "/Test/Path/To/Asset";
    const uint8_t speeddialTest = 100;
    const ContactAddressType addressTypeTest = ContactAddressType ::WORK;
    const ContactType contactTypeTest = ContactType ::USER;
    const ContactNumberType contactNumberTypeTest = ContactNumberType ::PAGER;


    ContactRecordInterface contRecInterface;

    ContactRecord recordIN;

    recordIN.primaryName=primaryNameTest;
    recordIN.alternativeName= alternativeNameTest;
    recordIN.numberUser=numberUserTest;
    recordIN.numberE164=numberE164Test;
    recordIN.numberType = contactNumberTypeTest;
    recordIN.contactType=contactTypeTest;
    recordIN.country=countryTest;
    recordIN.city=cityTest;
    recordIN.street=streetTest;
    recordIN.number=numberTest;
    recordIN.note=noteTest;
    recordIN.mail=mailTest;
    recordIN.addressType=addressTypeTest;
    recordIN.assetPath=assetPath;
    recordIN.speeddial=speeddialTest;



    REQUIRE(contRecInterface.Add(recordIN) == true);
    REQUIRE(contRecInterface.Add(recordIN) == true);
    REQUIRE(contRecInterface.Add(recordIN) == true);
    REQUIRE(contRecInterface.Add(recordIN) == true);

    {
        auto recordOUT = contRecInterface.GetByID(1);

        REQUIRE(recordOUT.dbID == 1);

        REQUIRE(recordOUT.primaryName == primaryNameTest);
        REQUIRE(recordOUT.alternativeName == alternativeNameTest);
        REQUIRE(recordOUT.numberUser == numberUserTest);
        REQUIRE(recordOUT.numberE164 == numberE164Test);
        REQUIRE(recordOUT.numberType == contactNumberTypeTest);
        REQUIRE(recordOUT.contactType == contactTypeTest);
        REQUIRE(recordOUT.country == countryTest);
        REQUIRE(recordOUT.city == cityTest);
        REQUIRE(recordOUT.street == streetTest);
        REQUIRE(recordOUT.number == numberTest);
        REQUIRE(recordOUT.note == noteTest);
        REQUIRE(recordOUT.mail == mailTest);
        REQUIRE(recordOUT.addressType == addressTypeTest);
        REQUIRE(recordOUT.assetPath == assetPath);
        REQUIRE(recordOUT.speeddial == speeddialTest);

    }

    {
        auto recordList = contRecInterface.GetLimitOffset(0,4);
        REQUIRE(recordList->size() == 4);

        uint32_t  cnt = 1;
        for(const auto w: *recordList){

            REQUIRE(w.dbID == cnt++);

            REQUIRE(w.primaryName == primaryNameTest);
            REQUIRE(w.alternativeName == alternativeNameTest);
            REQUIRE(w.numberUser == numberUserTest);
            REQUIRE(w.numberE164 == numberE164Test);
            REQUIRE(w.numberType == contactNumberTypeTest);
            REQUIRE(w.contactType == contactTypeTest);
            REQUIRE(w.country == countryTest);
            REQUIRE(w.city == cityTest);
            REQUIRE(w.street == streetTest);
            REQUIRE(w.number == numberTest);
            REQUIRE(w.note == noteTest);
            REQUIRE(w.mail == mailTest);
            REQUIRE(w.addressType == addressTypeTest);
            REQUIRE(w.assetPath == assetPath);
            REQUIRE(w.speeddial == speeddialTest);
        }



    }

    {
        auto recordList = contRecInterface.GetLimitOffsetByField(0,4,ContactRecordField::PrimaryName,primaryNameTest);
        REQUIRE(recordList->size() == 4);

        uint32_t  cnt = 1;
        for(const auto w: *recordList){

            REQUIRE(w.dbID == cnt++);

            REQUIRE(w.primaryName == primaryNameTest);
            REQUIRE(w.alternativeName == alternativeNameTest);
            REQUIRE(w.numberUser == numberUserTest);
            REQUIRE(w.numberE164 == numberE164Test);
            REQUIRE(w.numberType == contactNumberTypeTest);
            REQUIRE(w.contactType == contactTypeTest);
            REQUIRE(w.country == countryTest);
            REQUIRE(w.city == cityTest);
            REQUIRE(w.street == streetTest);
            REQUIRE(w.number == numberTest);
            REQUIRE(w.note == noteTest);
            REQUIRE(w.mail == mailTest);
            REQUIRE(w.addressType == addressTypeTest);
            REQUIRE(w.assetPath == assetPath);
            REQUIRE(w.speeddial == speeddialTest);
        }



    }

    {
    auto recordList = contRecInterface.GetLimitOffsetByField(0,2,ContactRecordField::NumberE164,numberE164Test);
    REQUIRE(recordList->size() == 2);

    uint32_t  cnt = 1;
    for(const auto w: *recordList){

        REQUIRE(w.dbID == cnt++);

        REQUIRE(w.primaryName == primaryNameTest);
        REQUIRE(w.alternativeName == alternativeNameTest);
        REQUIRE(w.numberUser == numberUserTest);
        REQUIRE(w.numberE164 == numberE164Test);
        REQUIRE(w.numberType == contactNumberTypeTest);
        REQUIRE(w.contactType == contactTypeTest);
        REQUIRE(w.country == countryTest);
        REQUIRE(w.city == cityTest);
        REQUIRE(w.street == streetTest);
        REQUIRE(w.number == numberTest);
        REQUIRE(w.note == noteTest);
        REQUIRE(w.mail == mailTest);
        REQUIRE(w.addressType == addressTypeTest);
        REQUIRE(w.assetPath == assetPath);
        REQUIRE(w.speeddial == speeddialTest);
    }



    }







    Database::Deinitialize();
}