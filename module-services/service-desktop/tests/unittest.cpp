// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Endpoint.hpp>
#include <endpoints/EndpointFactory.hpp>
#include <endpoints/contacts/ContactHelper.hpp>
#include <endpoints/contacts/ContactsEndpoint.hpp>
#include <endpoints/factoryReset/FactoryReset.hpp>
#include <endpoints/messages/MessageHelper.hpp>
#include <endpoints/update/UpdateMuditaOS.hpp>
#include <parser/ParserFSM.hpp>
#include <parser/ParserUtils.hpp>

#include <Common/Common.hpp>
#include <ContactRecord.hpp>
#include <PhoneNumber.hpp>
#include <SMSRecord.hpp>
#include <SMSTemplateRecord.hpp>
#include <catch2/catch.hpp>
#include <json/json11.hpp>
#include <purefs/filesystem_paths.hpp>
#include <utf8/UTF8.hpp>
#include <vfs.hpp>

#include <memory>
#include <filesystem>
#include <string>
#include <vector>

class vfs vfs;

TEST_CASE("System Update Tests")
{
    vfs.Init();

    UpdateMuditaOS updateOS(nullptr);

    updateos::UpdateError err = updateOS.prepareTempDirForUpdate();
    REQUIRE(err == updateos::UpdateError::NoError);

    updateOS.setUpdateFile("muditaos-unittest.tar");

    err = updateOS.unpackUpdate();
    REQUIRE(err == updateos::UpdateError::NoError);

    err = updateOS.verifyChecksums();
    REQUIRE(err == updateos::UpdateError::NoError);
}

using namespace parserFSM;

TEST_CASE("Parser Test")
{
    vfs.Init();

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
    vfs.Init();

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
        REQUIRE(contact.numbers.at(0).number.getEntered() == "724842187");
        REQUIRE(contact.primaryName == "Baatek");
    }
    SECTION("incorrect json")
    {
        std::string recordPayload =
            R"({"adress": "6 Czeczota St.\n02600 Warsaw", "altName": "Cic", "blocked": "true", "favourite": true, "numbers": "724842187", "priName": "Baatek"})";
        auto contactJson = json11::Json::parse(recordPayload, err);
        REQUIRE(err.empty());

        auto helper  = std::make_unique<ContactHelper>(nullptr);
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
    vfs.Init();

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
    vfs.Init();

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

TEST_CASE("Context class test")
{
    vfs.Init();

    SECTION("Correct message")
    {
        auto testMessage = R"({"endpoint":7, "method":1, "uuid":12345, "body":{"test":"test"}})";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        Context context(msgJson);
        REQUIRE(context.getBody()["test"] == "test");
        REQUIRE(context.getMethod() == http::Method::get);
        REQUIRE(context.getUuid() == 12345);
        REQUIRE(context.getEndpoint() == EndpointType::contacts);
        REQUIRE(context.createSimpleResponse() ==
                R"(#000000061{"body": null, "endpoint": 7, "status": 200, "uuid": "12345"})");

        context.setResponseBody(context.getBody());
        REQUIRE(context.createSimpleResponse() ==
                R"(#000000073{"body": {"test": "test"}, "endpoint": 7, "status": 200, "uuid": "12345"})");
    }
    SECTION("Invalid message")
    {
        auto testMessage = R"({"endpoint":25, "method":8, "uuid":"0", "body":{"te":"test"}})";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        Context context(msgJson);
        REQUIRE(context.getBody()["test"] == json11::Json());
        REQUIRE(context.getMethod() == http::Method::get);
        REQUIRE(context.getUuid() == invalidUuid);
        REQUIRE(context.getEndpoint() == EndpointType::invalid);
    }
}

TEST_CASE("Endpoint Factory test")
{
    vfs.Init();

    SECTION("Proper endpoint")
    {
        auto testMessage = R"({"endpoint":7, "method":1, "uuid":12345, "body":{"test":"test"}})";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        Context context(msgJson);
        auto handler = EndpointFactory::create(context, nullptr);
        REQUIRE(dynamic_cast<ContactsEndpoint *>(handler.get()));
    }

    SECTION("Wrong endpoint")
    {
        auto testMessage = R"({"endpoint":25, "method":8, "uuid":"12345", "body":{"te":"test"}})";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        Context context(msgJson);
        auto handler = EndpointFactory::create(context, nullptr);
        REQUIRE(handler == nullptr);
    }
}
