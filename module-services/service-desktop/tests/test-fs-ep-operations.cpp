// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Endpoint.hpp>
#include <endpoints/EndpointFactory.hpp>

#include <catch2/catch.hpp>
#include <json11.hpp>
#include <purefs/filesystem_paths.hpp>
#include <utf8/UTF8.hpp>

#include <memory>
#include <filesystem>
#include <string>
#include <vector>

std::vector<std::string> messageStrings;

xQueueHandle parserFSM::MessageHandler::sendQueue;

parserFSM::MessageHandler::MessageHandler(sys::Service *OwnerService, std::unique_ptr<EndpointFactory> endpointFactory)
    : OwnerServicePtr(OwnerService), endpointFactory(std::move(endpointFactory))
{}

void parserFSM::MessageHandler::parseMessage(const std::string &msg)
{}

void parserFSM::MessageHandler::processMessage()
{}

void parserFSM::MessageHandler::putToSendQueue(const std::string &msg)
{
    messageStrings.push_back(msg);
}
//~stubs

TEST_CASE("Endpoint Filesystem Test")
{
    messageStrings.clear();

    SECTION("Request send file")
    {
        auto endpoint    = 3;
        auto uuid        = 1103;
        auto fileToSend  = "\"/sys/user/data/applications/settings/quotes.json\"";
        auto fileSize    = 676u;
        auto fileCrc32   = "\"37ef9a52\"";
        auto chunkSize   = FileOperations::ChunkSize;
        auto txID        = 1u;
        auto testMessage = "{\"endpoint\":" + std::to_string(endpoint) +
                           ", \"method\": 3, \"uuid\":" + std::to_string(uuid) +
                           ", \"body\":{\"fileName\":" + fileToSend + " , \"fileSize\":" + std::to_string(fileSize) +
                           " , \"fileCrc32\":" + fileCrc32 + "}}";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        UNSCOPED_INFO(err.c_str());

        REQUIRE(err.empty());

        parserFSM::Context context(msgJson);
        auto factory = std::make_unique<SecuredEndpointFactory>(EndpointSecurity::Allow);
        auto handler = factory->create(context, nullptr);
        handler->handle(context);
        REQUIRE(1 == messageStrings.size());
        auto msg = messageStrings[0];
        REQUIRE(msg.size() > 10);
        auto retJson = json11::Json::parse(msg.substr(10), err);

        UNSCOPED_INFO(err.c_str());

        REQUIRE(err.empty());
        REQUIRE(uuid == retJson[parserFSM::json::uuid].int_value());
        REQUIRE(endpoint == retJson[parserFSM::json::endpoint].int_value());

        auto body = retJson[parserFSM::json::body];
        REQUIRE(chunkSize == static_cast<uint32_t>(body[parserFSM::json::filesystem::chunkSize].int_value()));
        REQUIRE(txID == static_cast<uint32_t>(body[parserFSM::json::filesystem::txID].int_value()));
    }

    SECTION("Request send file chunk")
    {
        auto endpoint = 3;
        auto uuid     = 2103;
        auto chunkNo  = 1u;
        auto txID     = 1u;
        auto data =
            "WwogIHsKICAgICJsYW5nIjoiZW5nbGlzaCIsCiAgICAiYXV0aG9yIjogIkJ1ZGRoYSIsCiAgICAicXVvdGUiOiAiRG8gbm90IGR3ZWxsIG"
            "luIHRoZSBwYXN0LCBkbyBub3QgZHJlYW0gb2YgdGhlIGZ1dHVyZSwgY29uY2VudHJhdGUgdGhlIG1pbmQgb24gdGhlIHByZXNlbnQgbW9t"
            "ZW50LiIKICB9LAogIHsKICAgICJsYW5nIjoiZW5nbGlzaCIsCiAgICAiYXV0aG9yIjogIlRhcmEgQnJhY2giLAogICAgInF1b3RlIjogIl"
            "RoZSBvbmx5IHdheSB0byBsaXZlIGlzIGJ5IGFjY2VwdGluZyBlYWNoIG1pbnV0ZSBhcyBhbiB1bnJlcGVhdGFibGUgbWlyYWNsZS4iCiAg"
            "fSwKICB7CiAgICAibGFuZyI6ImVuZ2xpc2giLAogICAgImF1dGhvciI6ICJSaWNoYXIgQmFjaCIsCiAgICAicXVvdGUiOiAiVGhlIHNpbX"
            "BsZXN0IHRoaW5ncyBhcmUgb2Z0ZW4gdGhlIHRydWVzdCIKICB9LAogIHsKICAgICJsYW5nIjoiZW5nbGlzaCIsCiAgICAiYXV0aG9yIjog"
            "IkVja2hhcnQgVG9sbGUiLAogICAgInF1b3RlIjogIkFsd2F5cyBzYXkgeWVzIHRvIHRoZSBwcmVzZW50IG1vbWVudC4gU2F5IHllcyB0by"
            "BsaWZlLiIKICB9LAogIHsKICAgICJsYW5nIjoiZW5nbGlzaCIsCiAgICAiYXV0aG9yIjogIk5hb21pIEp1ZGQiLAogICAgInF1b3RlIjog"
            "IlNsb3cgZG93biwgc2ltcGxpZnkgYW5kIGJlIGtpbmQiCiAgfQpdCg==";

        auto testMessage = "{\"endpoint\":" + std::to_string(endpoint) +
                           ", \"method\": 3, \"uuid\":" + std::to_string(uuid) +
                           ", \"body\":{\"txID\":" + std::to_string(txID) + ", \"chunkNo\":" + std::to_string(chunkNo) +
                           ", \"data\":" + "\"" + data + "\"" + "}}";

        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        UNSCOPED_INFO(err.c_str());

        REQUIRE(err.empty());

        parserFSM::Context context(msgJson);
        auto factory = std::make_unique<SecuredEndpointFactory>(EndpointSecurity::Allow);
        auto handler = factory->create(context, nullptr);
        handler->handle(context);
        REQUIRE(1 == messageStrings.size());
        auto msg = messageStrings[0];
        REQUIRE(msg.size() > 10);
        auto retJson = json11::Json::parse(msg.substr(10), err); // string length and go to real data
        UNSCOPED_INFO(err.c_str());

        REQUIRE(err.empty());
        REQUIRE(uuid == retJson[parserFSM::json::uuid].int_value());
        REQUIRE(endpoint == retJson[parserFSM::json::endpoint].int_value());

        auto body = retJson[parserFSM::json::body];
        REQUIRE(chunkNo == static_cast<uint32_t>(body[parserFSM::json::filesystem::chunkNo].int_value()));
        REQUIRE(txID == static_cast<uint32_t>(body[parserFSM::json::filesystem::txID].int_value()));
    }

    SECTION("Request get file")
    {
        auto endpoint    = 3;
        auto uuid        = 1103;
        auto fileToGet   = "\"/sys/user/data/applications/settings/quotes.json\"";
        auto fileSize    = 676u;
        auto chunkSize   = FileOperations::ChunkSize;
        auto rxID        = 2u;
        auto testMessage = "{\"endpoint\":" + std::to_string(endpoint) +
                           ", \"method\":1, \"uuid\":" + std::to_string(uuid) +
                           ", \"body\":{\"fileName\":" + fileToGet + "}}";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        UNSCOPED_INFO(err.c_str());
        REQUIRE(err.empty());

        parserFSM::Context context(msgJson);
        auto factory = std::make_unique<SecuredEndpointFactory>(EndpointSecurity::Allow);
        auto handler = factory->create(context, nullptr);
        handler->handle(context);
        REQUIRE(1 == messageStrings.size());
        auto msg = messageStrings[0];
        REQUIRE(msg.size() > 10);
        auto retJson = json11::Json::parse(msg.substr(10), err); // string length and go to real data
        UNSCOPED_INFO(err.c_str());
        REQUIRE(err.empty());
        REQUIRE(uuid == retJson[parserFSM::json::uuid].int_value());
        REQUIRE(endpoint == retJson[parserFSM::json::endpoint].int_value());

        auto body = retJson[parserFSM::json::body];
        REQUIRE(fileSize == static_cast<uint32_t>(body[parserFSM::json::fileSize].int_value()));
        REQUIRE(chunkSize == static_cast<uint32_t>(body[parserFSM::json::filesystem::chunkSize].int_value()));
        REQUIRE(rxID == static_cast<uint32_t>(body[parserFSM::json::filesystem::rxID].int_value()));
    }

    SECTION("Request get file chunk")
    {
        auto endpoint = 3;
        auto uuid     = 2013;
        auto chunkNo  = 1u;
        auto rxID     = 2u;
        auto testMessage =
            "{\"endpoint\":" + std::to_string(endpoint) + ", \"method\":1, \"uuid\":" + std::to_string(uuid) +
            ", \"body\":{\"rxID\":" + std::to_string(rxID) + ", \"chunkNo\":" + std::to_string(chunkNo) + "}}";

        auto data =
            "WwogIHsKICAgICJsYW5nIjoiZW5nbGlzaCIsCiAgICAiYXV0aG9yIjogIkJ1ZGRoYSIsCiAgICAicXVvdGUiOiAiRG8gbm90IGR3ZWxsIG"
            "luIHRoZSBwYXN0LCBkbyBub3QgZHJlYW0gb2YgdGhlIGZ1dHVyZSwgY29uY2VudHJhdGUgdGhlIG1pbmQgb24gdGhlIHByZXNlbnQgbW9t"
            "ZW50LiIKICB9LAogIHsKICAgICJsYW5nIjoiZW5nbGlzaCIsCiAgICAiYXV0aG9yIjogIlRhcmEgQnJhY2giLAogICAgInF1b3RlIjogIl"
            "RoZSBvbmx5IHdheSB0byBsaXZlIGlzIGJ5IGFjY2VwdGluZyBlYWNoIG1pbnV0ZSBhcyBhbiB1bnJlcGVhdGFibGUgbWlyYWNsZS4iCiAg"
            "fSwKICB7CiAgICAibGFuZyI6ImVuZ2xpc2giLAogICAgImF1dGhvciI6ICJSaWNoYXIgQmFjaCIsCiAgICAicXVvdGUiOiAiVGhlIHNpbX"
            "BsZXN0IHRoaW5ncyBhcmUgb2Z0ZW4gdGhlIHRydWVzdCIKICB9LAogIHsKICAgICJsYW5nIjoiZW5nbGlzaCIsCiAgICAiYXV0aG9yIjog"
            "IkVja2hhcnQgVG9sbGUiLAogICAgInF1b3RlIjogIkFsd2F5cyBzYXkgeWVzIHRvIHRoZSBwcmVzZW50IG1vbWVudC4gU2F5IHllcyB0by"
            "BsaWZlLiIKICB9LAogIHsKICAgICJsYW5nIjoiZW5nbGlzaCIsCiAgICAiYXV0aG9yIjogIk5hb21pIEp1ZGQiLAogICAgInF1b3RlIjog"
            "IlNsb3cgZG93biwgc2ltcGxpZnkgYW5kIGJlIGtpbmQiCiAgfQpdCg==";

        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        UNSCOPED_INFO(err.c_str());
        REQUIRE(err.empty());

        parserFSM::Context context(msgJson);
        auto factory = std::make_unique<SecuredEndpointFactory>(EndpointSecurity::Allow);
        auto handler = factory->create(context, nullptr);
        handler->handle(context);
        REQUIRE(1 == messageStrings.size());
        auto msg = messageStrings[0];
        REQUIRE(msg.size() > 10);
        auto retJson = json11::Json::parse(msg.substr(10), err);
        UNSCOPED_INFO(err.c_str());

        REQUIRE(err.empty());
        REQUIRE(uuid == retJson[parserFSM::json::uuid].int_value());
        REQUIRE(endpoint == retJson[parserFSM::json::endpoint].int_value());

        auto body = retJson[parserFSM::json::body];
        UNSCOPED_INFO(body.string_value());
        REQUIRE(chunkNo == static_cast<uint32_t>(body[parserFSM::json::filesystem::chunkNo].int_value()));
        REQUIRE(rxID == static_cast<uint32_t>(body[parserFSM::json::filesystem::rxID].int_value()));

        REQUIRE_THAT(body[parserFSM::json::filesystem::data].string_value(), Catch::Matchers::Contains(data));
    }
}
