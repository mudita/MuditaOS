#include "ContactRecord.hpp"
#include "ParserUtils.hpp"
#include "Service/Common.hpp"
#include "UpdatePureOS.hpp"
#include "FactoryReset.hpp"
#include "ParserStateMachine.hpp"
#include "EndpointHandler.hpp"
#include "service-desktop/ServiceDesktop.hpp"

#include "json/json11.hpp"
#include <vfs.hpp>
#include <catch2/catch.hpp>
#include <iostream>

class vfs vfs;

TEST_CASE("System Update Tests")
{
    vfs.Init();
    UpdatePureOS updateOS(nullptr);

    updateos::UpdateError err = updateOS.prepareTempDirForUpdate();
    REQUIRE(err == updateos::UpdateError::NoError);

    updateOS.setUpdateFile("pureos-unittest.tar");

    err = updateOS.unpackUpdate();
    REQUIRE(err == updateos::UpdateError::NoError);

    err = updateOS.verifyChecksums();
    REQUIRE(err == updateos::UpdateError::NoError);
}

TEST_CASE("Factory Reset Test")
{
    vfs.Init();

    std::string sysdir = purefs::dir::eMMC_disk;
    sysdir += "/factory-test/sys";
    std::string factorydir = sysdir + "/factory";
    REQUIRE(FactoryReset::DeleteDirContent(sysdir) == true);
    REQUIRE(FactoryReset::CopyDirContent(factorydir, sysdir) == true);
}

using namespace ParserStateMachine;

TEST_CASE("Parser Test")
{
    StateMachine parser(nullptr);
    std::string testMessage("#00000");

    SECTION("Parse message with divided header and payload")
    {
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialHeader);

        testMessage = R"(0050{"endpo)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialPayload);

        testMessage = R"(int":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }

    SECTION("Parse whole message")
    {
        testMessage = R"(#000000050{"endpoint":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }

    SECTION("Parse message with start char detached from mesage")
    {
        testMessage = R"(#)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialHeader);

        testMessage = R"(000000050{"en)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialPayload);

        testMessage = R"(dpoint":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }

    SECTION("Parse message with beginning of another one")
    {
        testMessage = R"(#000000050{"endpoint":1, "method":1, "body":{"test":"test"}}#000000050{"end)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialPayload);

        testMessage = R"(point":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }
    SECTION("Parse junk message")
    {
        testMessage = R"({"address": "6 Czeczota St.\n02600 Warsaw"})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::NoMsg);
    }
    SECTION("Parse message with incorrect header length")
    {
        testMessage = R"(#000000072{"endpoint":7, "method":2, "uuid":3, "body":{"threadID":1,"unread":false}})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::NoMsg);
    }
    SECTION("Parse message with damaged json ")
    {
        testMessage = R"(#000000074{"endpoint":7, "method":2, "uuid":3, "bo}}dy":{"threadID":1,"unread":false)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::NoMsg);
    }
    SECTION("Parse message with damaged json and incorrect header length")
    {
        testMessage = R"(#000000072{"endpoint":7, "method":2, "uuid":3, "bo}}dy":{"threadID":1,"unread":false)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::NoMsg);
    }
}

TEST_CASE("DB Helpers test - json decoding")
{
    std::string err;

    SECTION("correct json")
    {
        std::string recordPayload =
            R"({"address": "6 Czeczota St.\n02600 Warsaw", "altName": "Cic", "blocked": true, "favourite": true, "numbers": ["724842187"], "priName": "Baatek"})";
        auto contactJson = json11::Json::parse(recordPayload, err);
        REQUIRE(err.empty());

        auto helper = std::make_unique<ContactHelper>(nullptr);

        auto contact = helper->from_json(contactJson);
        REQUIRE(contact->address == "6 Czeczota St.\n02600 Warsaw");
        REQUIRE(contact->alternativeName == "Cic");
        REQUIRE(contact->isOnBlocked() == true);
        REQUIRE(contact->isOnFavourites() == true);
        REQUIRE(contact->numbers.at(0).number.getFormatted() == "724 842 187");
        REQUIRE(contact->primaryName == "Baatek");
    }
    SECTION("incorrect json")
    {
        std::string recordPayload =
            R"({"adress": "6 Czeczota St.\n02600 Warsaw", "altName": "Cic", "blocked": "true", "favourite": true, "numbers": "724842187", "priName": "Baatek"})";
        auto contactJson = json11::Json::parse(recordPayload, err);
        REQUIRE(err.empty());

        auto helper = std::make_unique<ContactHelper>(nullptr);

        auto contact = helper->from_json(contactJson);
        REQUIRE(contact->address == "");
        REQUIRE(contact->alternativeName == "Cic");
        REQUIRE(contact->isOnBlocked() == true);
        REQUIRE(contact->isOnFavourites() == true);
        REQUIRE(contact->numbers.empty());
        REQUIRE(contact->primaryName == "Baatek");
        REQUIRE(helper->createDBEntry(recordPayload, 123) == sys::ReturnCodes::Failure);
    }
}

TEST_CASE("DB Helpers test - json encoding")
{
    auto helper = std::make_unique<ContactHelper>(nullptr);

    auto contact             = std::make_unique<ContactRecord>();
    contact->address         = "6 Czeczota St.\n02600 Warsaw";
    contact->alternativeName = "Cic";
    contact->addToBlocked(true);
    contact->addToFavourites(true);
    contact->primaryName = "Baatek";

    utils::PhoneNumber phoneNumber("724842187");
    auto contactNum = ContactRecord::Number(phoneNumber.get(), phoneNumber.toE164(), ContactNumberType ::PAGER);

    contact->numbers.emplace_back(contactNum);

    auto contactJson = helper->to_json(*contact);

    REQUIRE(contactJson[json::contacts::address] == "6 Czeczota St.\n02600 Warsaw");
    REQUIRE(contactJson[json::contacts::alternativeName] == "Cic");
    REQUIRE(contactJson[json::contacts::isBlocked] == true);
    REQUIRE(contactJson[json::contacts::isFavourite] == true);
    REQUIRE(contactJson[json::contacts::primaryName] == "Baatek");
    REQUIRE(contactJson[json::contacts::numbers][0].dump() == "\"724842187\"");
}

TEST_CASE("Simple response builder test")
{
    auto resp =
        EndpointHandler::createSimpleResponse(sys::ReturnCodes::Success, static_cast<int>(Endpoint::contacts), 1234);
    std::string err;
    REQUIRE(resp.substr(0, 10) == "#000000058");
    resp.erase(0, 10);

    auto responseJson = json11::Json::parse(resp, err);
    REQUIRE(err.empty());
    REQUIRE(responseJson[json::endpoint] == static_cast<int>(Endpoint::contacts));
    REQUIRE(responseJson[json::body] == "");
    REQUIRE(responseJson[json::status] == static_cast<int>(http::Code::OK));
    REQUIRE(responseJson[json::uuid].dump() == "\"1234\"");
}
