// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "DTMFCode.hpp"

TEST_CASE("DTMFCode")
{
    SECTION("Parse from char")
    {
        auto code = DTMFCode('0');
        REQUIRE(code.getDigitASCIICode() == '0');
        REQUIRE(std::string(code) == "\"0\"");
    }
    SECTION("Parse from char 2")
    {
        auto code = DTMFCode('9');
        REQUIRE(code.getDigitASCIICode() == '9');
        REQUIRE(std::string(code) == "\"9\"");
    }
    SECTION("Parse from char 3")
    {
        auto code = DTMFCode('*');
        REQUIRE(code.getDigitASCIICode() == '*');
        REQUIRE(std::string(code) == "\"*\"");
    }
    SECTION("Parse from char 4")
    {
        auto code = DTMFCode('#');
        REQUIRE(code.getDigitASCIICode() == '#');
        REQUIRE(std::string(code) == "\"#\"");
    }

    SECTION("Parse from string")
    {
        auto code = DTMFCode("0");
        REQUIRE(code.getDigitASCIICode() == '0');
        REQUIRE(std::string(code) == "\"0\"");
    }
    SECTION("Parse from string 2")
    {
        auto code = DTMFCode("9");
        REQUIRE(code.getDigitASCIICode() == '9');
        REQUIRE(std::string(code) == "\"9\"");
    }
    SECTION("Parse from string 3")
    {
        auto code = DTMFCode("*");
        REQUIRE(code.getDigitASCIICode() == '*');
        REQUIRE(std::string(code) == "\"*\"");
    }
    SECTION("Parse from string 4")
    {
        auto code = DTMFCode("#");
        REQUIRE(code.getDigitASCIICode() == '#');
        REQUIRE(std::string(code) == "\"#\"");
    }

    SECTION("Parse from char - incorrect input")
    {
        REQUIRE_THROWS_AS(DTMFCode('a'), std::out_of_range);
    }
    SECTION("Parse from char - incorrect input 2")
    {
        REQUIRE_THROWS_AS(DTMFCode('!'), std::out_of_range);
    }
    SECTION("Parse from string - incorrect input 1")
    {
        REQUIRE_THROWS_AS(DTMFCode("a"), std::out_of_range);
    }
    SECTION("Parse from string - incorrect input 2")
    {
        REQUIRE_THROWS_AS(DTMFCode("!"), std::out_of_range);
    }
    SECTION("Parse from string - incorrect input 3")
    {
        REQUIRE_THROWS_AS(DTMFCode("12"), std::out_of_range);
    }
}
