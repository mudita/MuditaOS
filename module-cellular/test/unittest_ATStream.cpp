// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "Modem/ATStream.hpp"
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
}
