// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstring>
#include <iostream>
#include <memory>
#include <unistd.h>

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include <catch2/catch.hpp>

#include "Utils.hpp"

TEST_CASE("Split tests")
{
    std::string delimiter = "\r\n";

    std::string inputStr1 = "\r\nOK\r\n\r\nNO CARRIER\r\n\r\nFINISHED\r\n";
    auto ret              = utils::split(inputStr1, delimiter, false);
    REQUIRE(ret.size() == 7);
    REQUIRE(ret[0] == "");
    REQUIRE(ret[1] == "OK");
    REQUIRE(ret[2] == "");
    REQUIRE(ret[3] == "NO CARRIER");
    REQUIRE(ret[4] == "");
    REQUIRE(ret[5] == "FINISHED");
    REQUIRE(ret[6] == "");

    ret = utils::split(inputStr1, delimiter);
    REQUIRE(ret.size() == 3);
    REQUIRE(ret[0] == "OK");
    REQUIRE(ret[1] == "NO CARRIER");
    REQUIRE(ret[2] == "FINISHED");

    ret = utils::split("a-b-c", "-");
    REQUIRE(ret.size() == 3);
    REQUIRE(ret[0] == "a");
    REQUIRE(ret[1] == "b");
    REQUIRE(ret[2] == "c");

    ret = utils::split("a--c", "-", false);
    REQUIRE(ret.size() == 3);
    REQUIRE(ret[0] == "a");
    REQUIRE(ret[1] == "");
    REQUIRE(ret[2] == "c");

    ret = utils::split("a--c", "-");
    REQUIRE(ret.size() == 2);
    REQUIRE(ret[0] == "a");
    REQUIRE(ret[1] == "c");

    ret = utils::split("-b-", "-", false);
    REQUIRE(ret.size() == 3);
    REQUIRE(ret[0] == "");
    REQUIRE(ret[1] == "b");
    REQUIRE(ret[2] == "");

    ret = utils::split("-b-", "-");
    REQUIRE(ret.size() == 1);
    REQUIRE(ret[0] == "b");

    ret = utils::split("", "-", false);
    REQUIRE(ret.size() == 1);
    REQUIRE(ret[0] == "");

    ret = utils::split("", "-");
    REQUIRE(ret.size() == 0);
}

TEST_CASE("toNumeric tests")
{
    std::string inputStr1 = "2";
    uint32_t value;

    auto ret = utils::toNumeric(inputStr1, value);
    REQUIRE(ret == true);
    REQUIRE(value == 2);
}

TEST_CASE("findAndReplaceAll tests")
{
    // helper lambda
    auto compare = [](std::string &data, std::string &expected, bool &retVal) {
        if (data.compare(expected)) {
            std::cout << "Expected:" << std::endl
                      << "\t" << expected << std::endl
                      << "But is:" << std::endl
                      << "\t" << data << std::endl;
            retVal = false;
        }
    };

    // test findAndReplaceAll with different data combinations
    enum
    {
        eTestString,
        eToSearch,
        eReplace,
        eExpected
    };
    std::vector<std::vector<std::string>> testValues = {
        // multiple replacements
        {"%T TT %T TT %t %T", "%T", "test", "test TT test TT %t test"},
        {"%T TT %T TT %t %T", "%t", "test", "%T TT %T TT test %T"},
        // capital letter test
        {"%T TT %T TT %T %T", "%t", "test", "%T TT %T TT %T %T"},
        // empty test string
        {"", "%t", "test", ""},
        // no match
        {"TEST", "%t", "test", "TEST"},
        // empty match string
        {"TEST", "", "test", "TEST"},
        // empty test string and match string
        {"", "", "test", ""},
        // empty test, match and replace
        {"", "", "", ""},
        // empty replace string
        {"%T TT %T TT %T %T", " ", "", "%TTT%TTT%T%T"},
    };

    std::string testString;
    bool retVal = true;

    for (auto &testCase : testValues) {
        testString = testCase[eTestString];
        utils::findAndReplaceAll(testString, testCase[eToSearch], testCase[eReplace]);
        compare(testString, testCase[eExpected], retVal);
    }

    // test findAndReplaceAll with replacement function
    std::string testFormat, expectedFormat;
    std::vector<std::string> testValuesFunc = {"A", "B", "C", "D"};
    // test helper lambdas
    auto toLower     = [](unsigned char c) { return std::tolower(c); };
    auto replaceFunc = [&](int idx) { return std::string(1, toLower(testValuesFunc[idx][0])); };

    // create test format
    for (const auto &ch : testValuesFunc) {
        testFormat += ch;
    }
    // create expected result
    expectedFormat = testFormat;
    std::transform(expectedFormat.begin(), expectedFormat.end(), expectedFormat.begin(), [toLower](unsigned char c) {
        return toLower(c);
    });

    utils::findAndReplaceAll(testFormat, testValuesFunc, replaceFunc);
    compare(testFormat, expectedFormat, retVal);

    REQUIRE(retVal == true);
}

TEST_CASE("Converts enum to string")
{
    enum class Test : bool
    {
        True  = true,
        False = false
    };

    REQUIRE(utils::enumToString(Test::True) == "True");
    REQUIRE(utils::enumToString(Test::False) == "False");
}

TEST_CASE("Get value from string")
{
    SECTION("UInt32_t")
    {
        std::string testString = "10";
        const auto testValue   = utils::getValue<uint32_t>(testString);
        REQUIRE(testValue == 10);
    }

    SECTION("float")
    {
        std::string testString = "1.f";
        const auto testValue   = utils::getValue<float>(testString);
        Approx target          = Approx(1.f).margin(.01f);
        REQUIRE(testValue == target);
    }
}

TEST_CASE("Swap endianness")
{
    uint32_t as_long = 0x11223344;

    SECTION("endiannes check")
    {
        uint8_t *as_array;
        as_array = reinterpret_cast<uint8_t *>(&as_long);
        if (as_array[0] == 0x11) {
            REQUIRE(BYTE_ORDER == BIG_ENDIAN);
        }
        else if (as_array[0] == 0x44) {
            REQUIRE(BYTE_ORDER == LITTLE_ENDIAN);
        }
        else {
            FAIL("cannot determine endiannes");
        }
    }
    SECTION("swap endiannes uint32")
    {
        uint32_t as_long_swapped = utils::swapBytes(as_long);
        REQUIRE(((as_long >> 8 * 3) & 0xFF) == ((as_long_swapped >> 8 * 0) & 0xFF));
        REQUIRE(((as_long >> 8 * 2) & 0xFF) == ((as_long_swapped >> 8 * 1) & 0xFF));
        REQUIRE(((as_long >> 8 * 1) & 0xFF) == ((as_long_swapped >> 8 * 2) & 0xFF));
        REQUIRE(((as_long >> 8 * 0) & 0xFF) == ((as_long_swapped >> 8 * 3) & 0xFF));
    }
}
