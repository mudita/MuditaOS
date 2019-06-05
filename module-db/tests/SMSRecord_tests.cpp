
/*
 * @file SMSRecord_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 05.06.19
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
#include "../Databases/ContactsDB.hpp"
#include "../Databases/SmsDB.hpp"
#include "../Interface/SMSRecord.hpp"

struct test{
    UTF8 test;
    UTF8 test2;
};


TEST_CASE("SMS Record tests")
{
    Database::Initialize();

    vfs.remove(ContactsDB::GetDBName());
    vfs.remove(SmsDB::GetDBName());


    const uint32_t dateTest = 123456789;
    const uint32_t dateSentTest = 987654321;
    const uint32_t errorCodeTest = 555;
    const char *numberTest = "111222333";
    const char *bodyTest = "Test SMS Body";
    const bool isReadTest = true;
    const SMSType typeTest = SMSType ::DRAFT;


    SMSRecordInterface smsRecInterface;

    SMSRecord recordIN;
    recordIN.date = dateTest;
    recordIN.dateSent = dateSentTest;
    recordIN.errorCode = errorCodeTest;
    recordIN.number = numberTest;
    recordIN.body = bodyTest;
    recordIN.isRead = isReadTest;
    recordIN.type = typeTest;

    REQUIRE(smsRecInterface.Add(recordIN));

    REQUIRE(smsRecInterface.RemoveByID(1));

}