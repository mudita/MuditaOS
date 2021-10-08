// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp> // for AssertionHandler, operator""_catch_sr, SourceLineInfo, StringRef, REQUIRE, Section, SECTION, SectionInfo, TEST_CASE
#include <module-db/Interface/ContactRecord.hpp> // for ContactRecord
#include <service-db/DBServiceAPI.hpp> // for DBServiceAPI, DBServiceAPI::noError, DBServiceAPI::ContactVerificationError, DBServiceAPI::emptyContactError
#include <memory> // for allocator

TEST_CASE("DB_API")
{
    SECTION("DBServiceAPI::verifyContact emptyContactError")
    {
        ContactRecord testRecord;

        DBServiceAPI::ContactVerificationResult err = DBServiceAPI::verifyContact(nullptr, testRecord);
        REQUIRE(err == DBServiceAPI::ContactVerificationResult::emptyContact);

        testRecord.primaryName = "testName";
        err                    = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.primaryName.clear();
        REQUIRE(err == DBServiceAPI::ContactVerificationResult::success);

        testRecord.alternativeName = "testName";
        err                        = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.alternativeName.clear();
        REQUIRE(err == DBServiceAPI::ContactVerificationResult::success);

        // Here should be tested contact with single number, but it would require involving sys::bus.

        testRecord.mail = "test@mail.com";
        err             = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.mail.clear();
        REQUIRE(err == DBServiceAPI::ContactVerificationResult::success);

        testRecord.mail = "test@mail.com";
        err             = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.mail.clear();
        REQUIRE(err == DBServiceAPI::ContactVerificationResult::success);

        testRecord.speeddial = "1";
        err                  = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.speeddial.clear();
        REQUIRE(err == DBServiceAPI::ContactVerificationResult::emptyContact);

        testRecord.address = "test address";
        err                = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.address.clear();
        REQUIRE(err == DBServiceAPI::ContactVerificationResult::success);

        testRecord.note = "test note";
        err             = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.note.clear();
        REQUIRE(err == DBServiceAPI::ContactVerificationResult::success);

        err = DBServiceAPI::verifyContact(nullptr, testRecord);
        REQUIRE(err == DBServiceAPI::ContactVerificationResult::emptyContact);
    }
}
