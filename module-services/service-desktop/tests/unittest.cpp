#include "Common/Common.hpp"
#include "ContactRecord.hpp"
#include "Endpoint.hpp"
#include "MessageType.hpp"
#include "ParserUtils.hpp"
#include "Service/Common.hpp"
#include "UpdatePureOS.hpp"
#include "FactoryReset.hpp"
#include "ParserStateMachine.hpp"
#include "contacts/ContactHelper.hpp"
#include "messages/MessageHelper.hpp"
#include "queries/sms/QuerySMSSearchByType.hpp"
#include "service-desktop/ServiceDesktop.hpp"

#include "json/json11.hpp"
#include <memory>
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
        REQUIRE(contact.address == "6 Czeczota St.\n02600 Warsaw");
        REQUIRE(contact.alternativeName == "Cic");
        REQUIRE(contact.isOnBlocked() == true);
        REQUIRE(contact.isOnFavourites() == true);
        REQUIRE(contact.numbers.at(0).number.getFormatted() == "724 842 187");
        REQUIRE(contact.primaryName == "Baatek");
    }
    SECTION("incorrect json")
    {
        std::string recordPayload =
            R"({"adress": "6 Czeczota St.\n02600 Warsaw", "altName": "Cic", "blocked": "true", "favourite": true, "numbers": "724842187", "priName": "Baatek"})";
        auto contactJson = json11::Json::parse(recordPayload, err);
        REQUIRE(err.empty());

        auto helper = std::make_unique<ContactHelper>(nullptr);
        auto contact = helper->from_json(contactJson);
        REQUIRE(contact.address == "");
        REQUIRE(contact.alternativeName == "Cic");
        REQUIRE(contact.isOnBlocked() == true);
        REQUIRE(contact.isOnFavourites() == true);
        REQUIRE(contact.numbers.empty());
        REQUIRE(contact.primaryName == "Baatek");
    }
}

TEST_CASE("DB Helpers test - json encoding (contacts)")
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

TEST_CASE("DB Helpers test - json encoding (messages)")
{
    auto helper  = std::make_unique<MessageHelper>(nullptr);
    auto message = std::make_unique<SMSRecord>();

    utils::PhoneNumber phoneNumber("111222333");
    auto contactNum = ContactRecord::Number(phoneNumber.get(), phoneNumber.toE164(), ContactNumberType ::PAGER);

    message->body      = "test message";
    message->contactID = 1;
    message->date      = 12345;
    message->dateSent  = 54321;
    message->errorCode = 0;
    message->number    = contactNum.number;
    message->threadID  = 1;
    message->ID        = 10;
    message->type      = SMSType::DRAFT;

    auto messageJson = helper->to_json(*message);

    REQUIRE(messageJson[json::messages::messageBody] == "test message");
    REQUIRE(messageJson[json::messages::contactID] == 1);
    REQUIRE(messageJson[json::messages::date] == 12345);
    REQUIRE(messageJson[json::messages::dateSent] == 54321);
    REQUIRE(messageJson[json::messages::threadID] == 1);
    REQUIRE(messageJson[json::messages::id] == 10);

    auto messageTemplate = std::make_unique<SMSTemplateRecord>();

    messageTemplate->text = "test template";
    messageTemplate->ID   = 1;

    auto messageTemplateJson = helper->to_json(*messageTemplate);

    REQUIRE(messageTemplateJson[json::messages::templateText] == "test template");
    REQUIRE(messageTemplateJson[json::messages::id] == 1);
}

TEST_CASE("Simple response builder test")
{
    auto resp = Endpoint::createSimpleResponse(
        sys::ReturnCodes::Success, static_cast<int>(EndpointType::contacts), 1234, json11::Json());
    std::string err;
    REQUIRE(resp.substr(0, 10) == "#000000060");
    resp.erase(0, 10);

    auto responseJson = json11::Json::parse(resp, err);
    REQUIRE(err.empty());
    REQUIRE(responseJson[json::endpoint] == static_cast<int>(EndpointType::contacts));
    REQUIRE(responseJson[json::status] == static_cast<int>(http::Code::OK));
    REQUIRE(responseJson[json::uuid].dump() == "\"1234\"");
}
