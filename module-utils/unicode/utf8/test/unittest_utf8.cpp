// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include "utf8/UTF8.hpp"

TEST_CASE("UTF8: operator index returns value")
{
    UTF8 ustr = UTF8("Rąbać");

    REQUIRE((uint32_t)('R') == ustr[0]);
    // Test "ą"
    REQUIRE(0x00000105 == ustr[1]);
}

TEST_CASE("UTF8: operator index exceeds string size")
{
    UTF8 ustr = UTF8("Rąbać");

    REQUIRE(0 == ustr[ustr.length() + 1]);
}

TEST_CASE("UTF8: operator== returns properly")
{
    const UTF8 test_test_test = "test test test";
    const UTF8 test_not       = "test not";
    const UTF8 test_test      = "test test";

    REQUIRE(test_test_test == test_test_test); // text exactly the same
    REQUIRE(test_not != test_test_test);       // different text totally
    REQUIRE(test_test != test_test_test);      // text 1 shorter than 2nd (with same text at the begining)
    REQUIRE(test_test_test != test_test);      // text 2 shorter than 1st (with same text at the begining)
}

TEST_CASE("UTF8: substr returns empty string when zero length is passed")
{
    UTF8 sourceString = UTF8("Rąbać drewno siekierą");

    UTF8 destinationString = sourceString.substr(1, 0);

    REQUIRE(destinationString.length() == 0);
}

TEST_CASE("UTF8: substr returns empty string when length is exceeded")
{
    UTF8 sourceString = UTF8("Rąbać drewno siekierą");

    uint32_t length        = sourceString.length() + 5;
    UTF8 destinationString = sourceString.substr(1, length);

    REQUIRE(destinationString.length() == 0);
}

TEST_CASE("UTF8: substr returns proper begin char")
{
    UTF8 sourceString      = UTF8("Rąbać drewno siekierą");
    UTF8 destinationString = sourceString.substr(0, 5);

    REQUIRE(destinationString[0] == sourceString[0]);
}

TEST_CASE("UTF8: substr returns proper string length")
{
    UTF8 sourceString = UTF8("Rąbać drewno siekierą");
    uint32_t length   = sourceString.length();
    uint32_t position = 0;
    while (length > 1) {
        UTF8 destinationString = sourceString.substr(position, length);
        REQUIRE(destinationString.length() == length);
        position++;
        length--;
    }
}

TEST_CASE("UTF8: find returns npos if not found")
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "E";
    REQUIRE(UTF8::npos == sourceString.find(to_find));
}

TEST_CASE("UTF8: find returns npos if pos exceeds string length")
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "A";
    REQUIRE(UTF8::npos == sourceString.find(to_find, sourceString.length() + 5));
}

TEST_CASE("UTF8: find returns position of passed string")
{
    std::string base("AaBbCcŃń");
    UTF8 sourceString = UTF8("AaBbCcŃń");

    const char *to_find = "Ń";
    auto base_position  = base.find(to_find);

    REQUIRE(base_position == sourceString.find(to_find));
}

TEST_CASE("UTF8: find returns position of passed string when pos is passed")
{
    std::string base("Aa Bb aCcŃń");
    UTF8 sourceString = UTF8("Aa Bb aCcŃń");

    const char *to_find = "a";
    auto base_position  = base.find(to_find, 3);

    REQUIRE(base_position == sourceString.find(to_find, 3));
}

TEST_CASE("UTF8: findLast returns npos if not found")
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "E";
    REQUIRE(UTF8::npos == sourceString.findLast(to_find, sourceString.length() - 1));
}

TEST_CASE("UTF8: findLast returns npos if pos exceeds string length")
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "A";
    REQUIRE(UTF8::npos == sourceString.findLast(to_find, sourceString.length() + 5));
}

TEST_CASE("UTF8: findLast returns position of passed string")
{
    std::string base("AaBbCcŃń");
    UTF8 sourceString = UTF8("AaBbCcŃń");

    const char *to_find = "Ń";
    auto base_position  = base.find(to_find);

    REQUIRE(base_position == sourceString.findLast(to_find, sourceString.length() - 1));
}

TEST_CASE("UTF8: findLast returns position of passed string when pos is passed")
{
    std::string base("Aa Bb aCcŃń");
    UTF8 sourceString = UTF8("Aa Bb aCcŃń");

    const char *to_find         = "a";
    std::uint32_t base_position = 1;

    REQUIRE(base_position == sourceString.findLast(to_find, 3));
}

TEST_CASE("UTF8: split returns empty object if idx exceeds string length")
{
    UTF8 sourceString("String testowy PODZIAŁ string testowy");

    UTF8 destinationString = sourceString.split(sourceString.length() + 10);
    REQUIRE(0 == destinationString.length());
}

TEST_CASE("UTF8: split returns propper string")
{
    UTF8 sourceString("String testowy PODZIAŁ string testowy");
    UTF8 expected("PODZIAŁ string testowy");

    int32_t divisionIdx    = sourceString.find("PODZIAŁ");
    UTF8 destinationString = sourceString.split(divisionIdx);
    REQUIRE(expected == destinationString);
}

TEST_CASE("UTF8: split strings have propper length after split")
{
    UTF8 sourceString("String testowy Ąą Ćć ńźżę PODZIAŁ string testowy Łłódź ");
    UTF8 expectedSource("String testowy Ąą Ćć ńźżę ");
    UTF8 expectedDestination("PODZIAŁ string testowy Łłódź ");

    int32_t divisionIdx = sourceString.find("PODZIAŁ");

    UTF8 destinationString = sourceString.split(divisionIdx);

    REQUIRE(expectedSource.length() == sourceString.length());
    REQUIRE(expectedDestination.length() == destinationString.length());
}

TEST_CASE("UTF8: split summary length is equal after split")
{
    UTF8 sourceString("String testowy Ąą Ćć ńźżę PODZIAŁ string testowy Łłódź ");
    int32_t divisionIdx = sourceString.find("PODZIAŁ");

    uint32_t lengthBeforeSplit = sourceString.length();

    UTF8 destinationString = sourceString.split(divisionIdx);

    uint32_t destinationStringLength = destinationString.length();
    uint32_t sourceStringLesngth     = sourceString.length();

    REQUIRE(lengthBeforeSplit == destinationStringLength + sourceStringLesngth);
}

TEST_CASE("UTF8: getLine returns empty string if its not line")
{
    UTF8 sourceString("ĄŚĆćśą Pierwsza linia Druga linia ");

    UTF8 destinationString = sourceString.getLine();

    REQUIRE(destinationString == UTF8(""));
    REQUIRE(destinationString.length() == 0);
}

TEST_CASE("UTF8: getLine returns proper string when line ends with n")
{
    UTF8 sourceString("ĄŚĆćśą Pierwsza linia\n Druga linia ");

    UTF8 destinationString = sourceString.getLine();

    REQUIRE(destinationString == "ĄŚĆćśą Pierwsza linia");
}

TEST_CASE("UTF8: getLine returns proper string when line ends with r")
{
    UTF8 sourceString("ĄŚĆćśą Pierwsza linia\r Druga linia ");

    UTF8 destinationString = sourceString.getLine();

    REQUIRE(destinationString == "ĄŚĆćśą Pierwsza linia");
}

TEST_CASE("UTF8: removeChar returns false when string to remove exceed")
{
    UTF8 sourceString("Teścik");

    bool retVal = sourceString.removeChar(2, sourceString.length() + 1);
    REQUIRE_FALSE(retVal);
}

TEST_CASE("UTF8: removeChar returns false when pos to remove exceed")
{
    UTF8 sourceString("Teścik");

    bool retVal = sourceString.removeChar(sourceString.length() + 1, 2);
    REQUIRE_FALSE(retVal);
}

TEST_CASE("UTF8: removeChar returns propper string")
{
    UTF8 sourceString("Teścik");
    UTF8 toCompare("Tecik");
    // remove 'ś'
    bool retVal = sourceString.removeChar(2, 1);
    REQUIRE(retVal);
    REQUIRE(toCompare.length() == sourceString.length());
    REQUIRE(toCompare.used() == sourceString.used());
}

TEST_CASE("UTF8: getChar")
{
    UTF8 fin = "Zadzwonię później, walczę z ostrym cieniem mgły ;)";
    UTF8 end = "";

    for (unsigned int i = 0; i < fin.length(); ++i) {
        end.insert((const char *)fin.getChar(i).utf8);
    }

    REQUIRE(fin == end);
}

// getChar returns U8char, [] returns u16 encoded value
TEST_CASE("UTF8: encode / decode / how it works")
{
    std::string test                = "ę";
    uint32_t test_u8char_len        = 0;
    const uint32_t code_encoded_u8t = 0xC499; // code encoded using utf8 notation
    const uint32_t code_encoded_uni = 0x0119; // code encoded using U+ notation -> Ux0119 (utf16)

    uint32_t test_code_decoded_u16 = UTF8::decode(test.c_str(), test_u8char_len);

    REQUIRE(test_code_decoded_u16 == code_encoded_uni);

    uint32_t test_code_encode_u8     = 0;
    uint32_t test_code_encode_u8_len = 0;
    bool result = UTF8::encode(test_code_decoded_u16, test_code_encode_u8, test_code_encode_u8_len);

    REQUIRE(result == true);
    REQUIRE(test_code_encode_u8 == code_encoded_u8t);

    UTF8 u8t_test = "ę";

    REQUIRE(code_encoded_uni == u8t_test[0]);
}

TEST_CASE("UTF8 bad case scenario - operator[] returns")
{
    UTF8 test                       = "ę";
    UTF8 next                       = "";
    const uint32_t code_encoded_uni = 0x0119; // code encoded using U+ notation -> Ux0119 (utf16)

    // assert that there is utf8 in string - who says it has to be?
    REQUIRE(*test.c_str() == 0xc4);
    REQUIRE(*(test.c_str() + 1) == 0x99);

    // get utf16 value for first character
    auto code_utf16 = test[0];
    REQUIRE(code_utf16 == code_encoded_uni);

    // insert it with insert code
    next.insertCode(code_utf16);

    REQUIRE(test == next);
}

TEST_CASE("U8char && UTF8: encode")
{
    // this don't work...
    // encode(0x119) as ę
    UTF8 val;
    uint32_t code       = 0x119;
    uint32_t len        = 0;
    uint32_t output     = 0;
    const uint32_t pl_e = 0xc499;
    bool res            = UTF8::encode(code, output, len);
    // check that encode works
    REQUIRE(len == 2);
    REQUIRE(res);
    REQUIRE(output == pl_e);
    auto u = U8char(code);
    REQUIRE(u.size == len);
}

TEST_CASE("UTF8: insert whole string which doesn't work")
{
    unsigned int len = 50;
    UTF8 lol;
    UTF8 fin = "Zadzwonię później, walczę z ostrym cieniem mgły ;)";
    for (unsigned int i = 0; i < fin.length(); ++i) {
        union
        {
            char ch[4] = {0};
            uint32_t code;
        } tmp;
        tmp.code = fin[i];
        lol.insertCode(tmp.code);
    }
    REQUIRE(lol == fin);
    REQUIRE(lol.length() == len);
}

TEST_CASE("UTF8: Convert to ascii if is ascii combination")
{
    UTF8 combination = "778568738465";
    REQUIRE(combination.isASCIICombination());
    REQUIRE(combination.toASCII() == std::string("MUDITA"));
}

TEST_CASE("UTF8: Not ASCII combination")
{
    SECTION("Pl number - 9 digits")
    {
        UTF8 combination = "600123456";
        REQUIRE_FALSE(combination.isASCIICombination());
        REQUIRE_FALSE(combination.toASCII().has_value());
    }

    SECTION("Pl e164 number")
    {
        UTF8 combination = "+48600123456";
        REQUIRE_FALSE(combination.isASCIICombination());
        REQUIRE_FALSE(combination.toASCII().has_value());
    }

    SECTION("Pl formatted number")
    {
        UTF8 combination = "600 123 456";
        REQUIRE_FALSE(combination.isASCIICombination());
        REQUIRE_FALSE(combination.toASCII().has_value());
    }

    SECTION("Pl formatted int number")
    {
        UTF8 combination = "+48 600 123 456";
        REQUIRE_FALSE(combination.isASCIICombination());
        REQUIRE_FALSE(combination.toASCII().has_value());
    }
}
