
/*
 * @file ThreadRecord_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 11.06.19
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
#include <Interface/ContactRecord.hpp>

#include "../Database/Database.hpp"
#include "../Databases/ContactsDB.hpp"
#include "../Databases/SmsDB.hpp"
#include "../Interface/SMSRecord.hpp"
#include "../Interface/ThreadRecord.hpp"

TEST_CASE("Thread Record tests")
{
    Database::Initialize();

    vfs.remove(ContactsDB::GetDBName());
    vfs.remove(SmsDB::GetDBName());

    auto smsDB      = std::make_unique<SmsDB>();
    auto contactsDB = std::make_unique<ContactsDB>();

    const uint32_t dateTest      = 123456789;
    const char *snippetTest      = "Test snippet";
    const char *snippetTest2     = "Test snippet2";
    const SMSType typeTest       = SMSType ::ALL;
    const uint32_t contactIDTest = 100;

    ThreadRecordInterface threadRecordInterface1(smsDB.get(), contactsDB.get());

    ThreadRecord recordIN;
    recordIN.date      = dateTest;
    recordIN.snippet   = snippetTest;
    recordIN.type      = typeTest;
    recordIN.contactID = contactIDTest;

    // Add 2 records
    REQUIRE(threadRecordInterface1.Add(recordIN));
    REQUIRE(threadRecordInterface1.Add(recordIN));

    // Get all available records
    auto records = threadRecordInterface1.GetLimitOffset(0, 100);
    REQUIRE((*records).size() == 2);

    // Check if fetched records contain valid data
    for (const auto &w : *records) {
        REQUIRE(w.date == dateTest);
        REQUIRE(w.snippet == snippetTest);
        REQUIRE(w.type == typeTest);
        REQUIRE(w.contactID == contactIDTest);
    }

    // Get all available records by specified thread ID and check for invalid data
    records = threadRecordInterface1.GetLimitOffsetByField(
        0, 100, ThreadRecordField ::ContactID, std::to_string(contactIDTest).c_str());
    REQUIRE((*records).size() == 2);
    for (const auto &w : *records) {
        REQUIRE(w.date == dateTest);
        REQUIRE(w.snippet == snippetTest);
        REQUIRE(w.type == typeTest);
        REQUIRE(w.contactID == contactIDTest);
    }

    // Remove records one by one
    REQUIRE(threadRecordInterface1.RemoveByID(1));
    REQUIRE(threadRecordInterface1.RemoveByID(2));

    // SMS database should not contain any records
    REQUIRE(threadRecordInterface1.GetCount() == 0);

    // Test updating record
    REQUIRE(threadRecordInterface1.Add(recordIN));
    recordIN.dbID    = 1;
    recordIN.snippet = snippetTest2;
    REQUIRE(threadRecordInterface1.Update(recordIN));

    auto record = threadRecordInterface1.GetByID(1);
    REQUIRE(record.dbID != 0);
    REQUIRE(record.snippet == snippetTest2);

    // SMS database should contain 1 record
    REQUIRE(threadRecordInterface1.GetCount() == 1);

    // Remove existing record
    REQUIRE(threadRecordInterface1.RemoveByID(1));
    // SMS database should be empty
    REQUIRE(threadRecordInterface1.GetCount() == 0);

    Database::Deinitialize();
}