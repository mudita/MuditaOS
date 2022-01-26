// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>

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
    int value;

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
        const auto testValue   = utils::getNumericValue<uint32_t>(testString);
        REQUIRE(testValue == 10);
    }

    SECTION("float")
    {
        std::string testString = "1.f";
        const auto testValue   = utils::getNumericValue<float>(testString);
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

TEST_CASE("Floating point to string")
{
    SECTION("Double")
    {
        double test = 15.0965432456321;
        REQUIRE(utils::to_string(test) == "15.096543");
    }

    SECTION("Negative double")
    {
        double test = -15.0965432456321;
        REQUIRE(utils::to_string(test) == "-15.096543");
    }

    SECTION("Double between 0 and 1")
    {
        double test = 0.08654328765876;
        REQUIRE(utils::to_string(test) == "0.086543");
    }

    SECTION("Double between -1 and 0")
    {
        double test = -0.08654328765876;
        REQUIRE(utils::to_string(test) == "-0.086543");
    }

    SECTION("Float")
    {
        float test = 15.0543212;
        REQUIRE(utils::to_string(test) == "15.054321");
    }

    SECTION("Negative float")
    {
        float test = -15.0543212;
        REQUIRE(utils::to_string(test) == "-15.054321");
    }

    SECTION("Float between 0 and 1")
    {
        float test = 0.0453212;
        REQUIRE(utils::to_string(test) == "0.045321");
    }

    SECTION("Float between -1 and 0")
    {
        float test = -0.0453212;
        REQUIRE(utils::to_string(test) == "-0.045321");
    }
}

TEST_CASE("Integer types to string")
{
    GIVEN("Uint8 equals to zero")
    {
        constexpr std::uint8_t value = 0;
        REQUIRE(utils::to_string(value) == "0");
    }
    GIVEN("Uint8 equals to max")
    {
        constexpr std::uint8_t value = std::numeric_limits<std::uint8_t>::max();
        REQUIRE(utils::to_string(value) == "255");
    }
    GIVEN("Int8 equals to zero")
    {
        constexpr std::int8_t value = 0;
        REQUIRE(utils::to_string(value) == "0");
    }
    GIVEN("Int8 equals to max")
    {
        constexpr std::int8_t value = std::numeric_limits<std::int8_t>::max();
        REQUIRE(utils::to_string(value) == "127");
    }
    GIVEN("Uint16t equals to zero")
    {
        constexpr std::uint16_t value = 0;
        REQUIRE(utils::to_string(value) == "0");
    }
    GIVEN("Uint16t equals to max")
    {
        constexpr std::uint16_t value = std::numeric_limits<std::uint16_t>::max();
        REQUIRE(utils::to_string(value) == "65535");
    }
    GIVEN("Int16t equals to zero")
    {
        constexpr std::int16_t value = 0;
        REQUIRE(utils::to_string(value) == "0");
    }
    GIVEN("Int16t equals to max")
    {
        constexpr std::int16_t value = std::numeric_limits<std::int16_t>::max();
        REQUIRE(utils::to_string(value) == "32767");
    }
    GIVEN("Uint32t equals to zero")
    {
        constexpr std::uint32_t value = 0;
        REQUIRE(utils::to_string(value) == "0");
    }
    GIVEN("Uint32 equals to max")
    {
        constexpr std::uint32_t value = std::numeric_limits<std::uint32_t>::max();
        REQUIRE(utils::to_string(value) == "4294967295");
    }
    GIVEN("Int32t equals to zero")
    {
        constexpr std::int32_t value = 0;
        REQUIRE(utils::to_string(value) == "0");
    }
    GIVEN("Int32 equals to max")
    {
        constexpr std::int32_t value = std::numeric_limits<std::int32_t>::max();
        REQUIRE(utils::to_string(value) == "2147483647");
    }
    GIVEN("Int64 equals to zero")
    {
        constexpr std::int64_t value = 0;
        REQUIRE(utils::to_string(value) == "0");
    }
    GIVEN("Int64 equals to max")
    {
        constexpr std::int64_t value = std::numeric_limits<std::int64_t>::max();
        REQUIRE(utils::to_string(value) == "9223372036854775807");
    }
    GIVEN("Uint64 equals to zero")
    {
        constexpr std::uint64_t value = 0;
        REQUIRE(utils::to_string(value) == "0");
    }
    GIVEN("Uint64 equals to max")
    {
        constexpr std::uint64_t value = std::numeric_limits<std::uint64_t>::max();
        REQUIRE(utils::to_string(value) == "18446744073709551615");
    }
}

TEST_CASE("Fill leading digit in string")
{
    std::string test = "45";
    REQUIRE(utils::addLeadingZeros(test) == "45");
    REQUIRE(utils::addLeadingZeros(test, 1) == "45");
    REQUIRE(utils::addLeadingZeros(test, 2) == "45");
    REQUIRE(utils::addLeadingZeros(test, 3) == "045");
    REQUIRE(utils::addLeadingZeros(test, 4) == "0045");
}

TEST_CASE("Hex to bytes")
{
    SECTION("One byte as two hex digits")
    {
        auto b = utils::hexToBytes("11");
        REQUIRE(b.size() == 1);
        REQUIRE(b[0] == 0x11);
    }

    SECTION("One byte as one hex digit")
    {
        auto b = utils::hexToBytes("2");
        REQUIRE(b.size() == 1);
        REQUIRE(b[0] == 2);
    }

    SECTION("Next numbers")
    {
        auto b = utils::hexToBytes("010203");
        REQUIRE(b.size() == 3);
        REQUIRE(b[0] == 1);
        REQUIRE(b[1] == 2);
        REQUIRE(b[2] == 3);
    }

    SECTION("Upper case")
    {
        auto b = utils::hexToBytes("DEADBEEF");
        REQUIRE(b.size() == 4);
        REQUIRE(b[0] == 0xDE);
        REQUIRE(b[1] == 0xAD);
        REQUIRE(b[2] == 0xBE);
        REQUIRE(b[3] == 0xEF);
    }

    SECTION("Lower case")
    {
        auto b = utils::hexToBytes("deadbeef");
        REQUIRE(b.size() == 4);
        REQUIRE(b[0] == 0xDE);
        REQUIRE(b[1] == 0xAD);
        REQUIRE(b[2] == 0xBE);
        REQUIRE(b[3] == 0xEF);
    }

    SECTION("Mixed case")
    {
        auto b = utils::hexToBytes("deAdbEEf");
        REQUIRE(b.size() == 4);
        REQUIRE(b[0] == 0xDE);
        REQUIRE(b[1] == 0xAD);
        REQUIRE(b[2] == 0xBE);
        REQUIRE(b[3] == 0xEF);
    }

    SECTION("Out of hex")
    {
        REQUIRE_THROWS_AS(utils::hexToBytes("deAdbEZZ"), std::invalid_argument);
    }
}

TEST_CASE("Bytes to hex")
{
    SECTION("Vector of bytes")
    {
        std::vector<std::uint8_t> vb = {1, 2, 3, 4, 0xFF};
        auto ret                     = utils::bytesToHex(vb);
        REQUIRE((ret == "01020304ff"));
    }
}
