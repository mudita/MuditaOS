#include <catch2/catch.hpp>
#include <module-db/Interface/ContactRecord.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>

TEST_CASE("DB_API")
{
    SECTION("DBServiceAPI::verifyContact emptyContactError")
    {
        ContactRecord testRecord;

        DBServiceAPI::ContactVerificationError err = DBServiceAPI::verifyContact(nullptr, testRecord);
        REQUIRE(err == DBServiceAPI::emptyContactError);

        testRecord.primaryName = "testName";
        err                    = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.primaryName.clear();
        REQUIRE(err == DBServiceAPI::noError);

        testRecord.alternativeName = "testName";
        err                        = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.alternativeName.clear();
        REQUIRE(err == DBServiceAPI::noError);

        // Here should be tested contact with single number, but it would require involving sys::bus.

        testRecord.mail = "test@mail.com";
        err             = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.mail.clear();
        REQUIRE(err == DBServiceAPI::noError);

        testRecord.mail = "test@mail.com";
        err             = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.mail.clear();
        REQUIRE(err == DBServiceAPI::noError);

        testRecord.speeddial = "1";
        err                  = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.speeddial.clear();
        REQUIRE(err == DBServiceAPI::emptyContactError);

        testRecord.address = "test address";
        err                = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.address.clear();
        REQUIRE(err == DBServiceAPI::noError);

        testRecord.note = "test note";
        err             = DBServiceAPI::verifyContact(nullptr, testRecord);
        testRecord.note.clear();
        REQUIRE(err == DBServiceAPI::noError);

        err = DBServiceAPI::verifyContact(nullptr, testRecord);
        REQUIRE(err == DBServiceAPI::emptyContactError);
    }
}
