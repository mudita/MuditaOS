﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Endpoint.hpp>
#include <endpoints/EndpointFactory.hpp>
#include <endpoints/contacts/ContactHelper.hpp>
#include <endpoints/contacts/ContactsEndpoint.hpp>
#include <endpoints/factoryReset/FactoryReset.hpp>
#include <endpoints/messages/MessageHelper.hpp>
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
#include <memory>
#include <filesystem>
#include <string>
#include <vector>

using namespace parserFSM;

TEST_CASE("Parser Test")
{
    StateMachine parser(nullptr);

    auto factory = std::make_unique<SecuredEndpointFactory>(EndpointSecurity::Allow);
    auto handler = std::make_unique<parserFSM::MessageHandler>(nullptr, std::move(factory));
    parser.setMessageHandler(std::move(handler));

    SECTION("Parse message with divided header and payload")
    {
        std::string testMessage("#00000");
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialHeader);

        testMessage = R"(0050{"endpo)";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialPayload);

        testMessage = R"(int":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }

    SECTION("Parse whole message")
    {
        std::string testMessage = R"(#000000050{"endpoint":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }

    SECTION("Parse message with start char detached from mesage")
    {
        std::string testMessage = R"(#)";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialHeader);

        testMessage = R"(000000050{"en)";
        testMessage = R"(000000050{"en)";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialPayload);

        testMessage = R"(dpoint":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }

    SECTION("Parse message with beginning of another one")
    {
        std::string testMessage = R"(#000000050{"endpoint":1, "method":1, "body":{"test":"test"}}#000000050{"end)";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialPayload);

        testMessage = R"(point":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }
    SECTION("Parse junk message")
    {
        std::string testMessage = R"({"address": "6 Czeczota St.\n02600 Warsaw"})";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::NoMsg);
    }
    SECTION("Parse message with incorrect header length")
    {
        std::string testMessage =
            R"(#000000072{"endpoint":7, "method":2, "uuid":3, "body":{"threadID":1,"unread":false}})";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::NoMsg);
    }
    SECTION("Parse message with damaged json ")
    {
        std::string testMessage =
            R"(#000000074{"endpoint":7, "method":2, "uuid":3, "bo}}dy":{"threadID":1,"unread":false)";
        parser.processMessage(std::move(testMessage));
        REQUIRE(parser.getCurrentState() == State::NoMsg);
    }
    SECTION("Parse message with damaged json and incorrect header length")
    {
        std::string testMessage =
            R"(#000000072{"endpoint":7, "method":2, "uuid":3, "bo}}dy":{"threadID":1,"unread":false)";
        parser.processMessage(std::move(testMessage));
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
    message->errorCode = 0;
    message->number    = contactNum.number;
    message->threadID  = 1;
    message->ID        = 10;
    message->type      = SMSType::DRAFT;

    auto messageJson = helper->toJson(*message);

    REQUIRE(messageJson[json::messages::messageBody] == "test message");
    REQUIRE(messageJson[json::messages::contactID] == 1);
    REQUIRE(messageJson[json::messages::createdAt] == 12345);
    REQUIRE(messageJson[json::messages::threadID] == 1);
    REQUIRE(messageJson[json::messages::messageID] == 10);

    auto messageTemplate = std::make_unique<SMSTemplateRecord>();

    messageTemplate->text = "test template";
    messageTemplate->ID   = 1;

    auto messageTemplateJson = helper->toJson(*messageTemplate);

    REQUIRE(messageTemplateJson[json::messages::templateBody] == "test template");
    REQUIRE(messageTemplateJson[json::messages::templateID] == 1);
}

TEST_CASE("Context class test")
{
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
                R"(#000000059{"body": null, "endpoint": 7, "status": 200, "uuid": 12345})");

        context.setResponseBody(context.getBody());
        REQUIRE(context.createSimpleResponse() ==
                R"(#000000071{"body": {"test": "test"}, "endpoint": 7, "status": 200, "uuid": 12345})");
    }
    SECTION("Invalid message")
    {
        auto testMessage = R"({"endpoint":25, "method":8, "uuid":0, "body":{"te":"test"}})";
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
    SECTION("Proper endpoint")
    {
        auto testMessage = R"({"endpoint":7, "method":1, "uuid":12345, "body":{"test":"test"}})";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        Context context(msgJson);
        auto factory = std::make_unique<EndpointFactory>();
        auto handler = factory->create(context, nullptr);
        REQUIRE(dynamic_cast<ContactsEndpoint *>(handler.get()));
    }

    SECTION("Wrong endpoint")
    {
        auto testMessage = R"({"endpoint":25, "method":8, "uuid":12345, "body":{"te":"test"}})";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        Context context(msgJson);
        auto factory = std::make_unique<EndpointFactory>();
        auto handler = factory->create(context, nullptr);
        REQUIRE(handler == nullptr);
    }
}

TEST_CASE("Secured Endpoint Factory test")
{
    SECTION("Allowed endpoint")
    {
        auto testMessage = R"({"endpoint":7, "method":1, "uuid":12345, "body":{"test":"test"}})";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        Context context(msgJson);
        auto factory = std::make_unique<SecuredEndpointFactory>(EndpointSecurity::Allow);
        auto handler = factory->create(context, nullptr);
        REQUIRE(dynamic_cast<ContactsEndpoint *>(handler.get()));
    }

    SECTION("Secured endpoint")
    {
        auto testMessage = R"({"endpoint":25, "method":8, "uuid":12345, "body":{"te":"test"}})";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        Context context(msgJson);
        auto factory = std::make_unique<SecuredEndpointFactory>(EndpointSecurity::Block);
        auto handler = factory->create(context, nullptr);
        REQUIRE(dynamic_cast<SecuredEndpoint *>(handler.get()));
    }
}
