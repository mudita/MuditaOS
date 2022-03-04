// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <catch2/catch.hpp>
#include "modem/ATStream.hpp"
#include "Result.hpp"

TEST_CASE("Channel Test- AT return parser")
{
    SECTION("Parse AT - OK")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("OK", errcode);
        REQUIRE(code == at::Result::Code::OK);
    }

    SECTION("Parse AT - ERROR")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("ERROR", errcode);
        REQUIRE(code == at::Result::Code::ERROR);
    }

    SECTION("Parse AT - +CME ERROR: Valid")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("+CME ERROR: 33", errcode);
        REQUIRE(code == at::Result::Code::CME_ERROR);
        REQUIRE(errcode == 33);
    }

    SECTION("Parse AT - +CMS ERROR: Valid")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("+CMS ERROR: 33", errcode);
        REQUIRE(code == at::Result::Code::CMS_ERROR);
        REQUIRE(errcode == 33);
    }

    SECTION("Parse AT - +CMS ERROR: Invalid")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("+CMS ERROR: ssss", errcode);
        REQUIRE(code == at::Result::Code::PARSING_ERROR);
    }

    SECTION("Parse AT - +CMS ERROR: Invalid empty")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("+CMS ERROR:", errcode);
        REQUIRE(code == at::Result::Code::PARSING_ERROR);
    }

    SECTION("Parse AT - +CME ERROR: Valid")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("+CME ERROR: 33", errcode);
        REQUIRE(code == at::Result::Code::CME_ERROR);
        REQUIRE(errcode == 33);
    }

    SECTION("Parse AT - +CME ERROR: Invalid")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("+CME ERROR: ssss", errcode);
        REQUIRE(code == at::Result::Code::PARSING_ERROR);
    }

    SECTION("Parse AT - +CME ERROR: Invalid empty")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("+CME ERROR:", errcode);
        REQUIRE(code == at::Result::Code::PARSING_ERROR);
    }

    SECTION("Parse AT - Wrong data")
    {
        at::ATStream stream;
        uint32_t errcode = 0;
        auto code        = stream.parseState("+XME ERROR:", errcode);
        REQUIRE(code == at::Result::Code::NONE);
    }

    SECTION("Parse AT, OK on begin, two tokens")
    {
        std::string rawResponse(
            "\r\nOK\r\n\r\n+QPING: 0,\"172.217.20.206\",32,30,255\r\n\r\n+QPING: 0,\"172.217.20.206\",32,31,255\r\n");

        at::ATStream stream(3);
        stream.write(rawResponse);
        auto result = stream.getResult();

        REQUIRE(result.code == at::Result::Code::OK);
        REQUIRE(result.response.size() == 3);
        REQUIRE(result.response[0] == "OK");
        REQUIRE(result.response[1] == "+QPING: 0,\"172.217.20.206\",32,30,255");
        REQUIRE(result.response[2] == "+QPING: 0,\"172.217.20.206\",32,31,255");
    }

    SECTION("Parse AT, OK at end, two tokens")
    {
        std::string rawResponse("\r\n+QDAI: 1,0,0,3,0,1,1,1\r\n\r\nOK\r\n");

        at::ATStream stream(3);
        stream.write(rawResponse);
        auto result = stream.getResult();

        REQUIRE(result.code == at::Result::Code::OK);
        REQUIRE(result.response.size() == 2);
        REQUIRE(result.response[0] == "+QDAI: 1,0,0,3,0,1,1,1");
        REQUIRE(result.response[1] == "OK");
    }
}
