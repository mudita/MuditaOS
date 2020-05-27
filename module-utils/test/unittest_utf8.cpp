#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

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

TEST_CASE("UTF8: operator index last index clears")
{
    UTF8 ustr = UTF8("Rąbać");

    // last element - operator_index_iterations should be equal ustr.strLength - 1
    ustr[ustr.length() - 1];
    REQUIRE(ustr.length() - 1 == ustr.operator_index_iterations);
    // next to last element - operator_index_iterations should be equal ustr.strLength - 2
    // lastIndex should clear, lastIndex is lower than last time
    ustr[ustr.length() - 2];
    REQUIRE(ustr.length() - 2 == ustr.operator_index_iterations);
}

TEST_CASE("UTF8: operator index last index increments")
{
    UTF8 ustr = UTF8("Rąbać");

    for (uint32_t i = 1; i < ustr.length(); i++) {
        ustr[i];
        REQUIRE(1 == ustr.operator_index_iterations);
    }
}

TEST_CASE("UTF8: operator index last index counting")
{
    UTF8 ustr = UTF8("Rąbać");

    // first element - operator_index_iterations should be equal 0
    ustr[0];
    REQUIRE(0 == ustr.operator_index_iterations);
    // third element - operator_index_iterations should be equal 2
    ustr[2];
    REQUIRE(2 == ustr.operator_index_iterations);
    // first element - operator_index_iterations should be equal 0
    ustr[0];
    REQUIRE(0 == ustr.operator_index_iterations);
    // last element - operator_index_iterations should be equal ustr.strLength - 1
    ustr[ustr.length() - 1];
    REQUIRE(ustr.length() - 1 == ustr.operator_index_iterations);
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
    REQUIRE(-1 == sourceString.find(to_find));
}

TEST_CASE("UTF8: find returns npos if pos exceeds string length")
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "A";
    REQUIRE(-1 == sourceString.find(to_find, sourceString.length() + 5));
}

TEST_CASE("UTF8: find returns position of passed string")
{
    std::string base("AaBbCcŃń");
    UTF8 sourceString = UTF8("AaBbCcŃń");

    const char *to_find   = "Ń";
    int32_t base_position = base.find(to_find);

    REQUIRE(base_position == sourceString.find(to_find));
}

TEST_CASE("UTF8: find returns position of passed string when pos is passed")
{
    std::string base("Aa Bb aCcŃń");
    UTF8 sourceString = UTF8("Aa Bb aCcŃń");

    const char *to_find   = "a";
    int32_t base_position = base.find(to_find, 3);

    REQUIRE(base_position == sourceString.find(to_find, 3));
}

TEST_CASE("UTF8: findLast returns npos if not found")
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "E";
    REQUIRE(-1 == sourceString.findLast(to_find, sourceString.length() - 1));
}

TEST_CASE("UTF8: findLast returns npos if pos exceeds string length")
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "A";
    REQUIRE(-1 == sourceString.findLast(to_find, sourceString.length() + 5));
}

TEST_CASE("UTF8: findLast returns position of passed string")
{
    std::string base("AaBbCcŃń");
    UTF8 sourceString = UTF8("AaBbCcŃń");

    const char *to_find   = "Ń";
    int32_t base_position = base.find(to_find);

    REQUIRE(base_position == sourceString.findLast(to_find, sourceString.length() - 1));
}

TEST_CASE("UTF8: findLast returns position of passed string when pos is passed")
{
    std::string base("Aa Bb aCcŃń");
    UTF8 sourceString = UTF8("Aa Bb aCcŃń");

    const char *to_find   = "a";
    int32_t base_position = 1;

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
