

#include "gtest/gtest.h"
#include "utf8/UTF8.hpp"

TEST(UTF8, operator_index_returns_value)
{
    UTF8 ustr = UTF8("Rąbać");

    ASSERT_EQ((uint32_t)('R'), ustr[0]);
    // Test "ą"
    ASSERT_EQ(0x000085c4, ustr[1]);
}

TEST(UTF8, operator_index_exceeds_string_size)
{
    UTF8 ustr = UTF8("Rąbać");

    ASSERT_EQ(0, ustr[ustr.length() + 1]);
}

TEST(UTF8, operator_index_last_index_clears)
{
    UTF8 ustr = UTF8("Rąbać");

    // last element - operator_index_iterations should be equal ustr.strLength - 1
    ustr[ustr.length() - 1];
    ASSERT_EQ(ustr.length() - 1, ustr.operator_index_iterations);
    // next to last element - operator_index_iterations should be equal ustr.strLength - 2
    // lastIndex should clear, lastIndex is lower than last time
    ustr[ustr.length() - 2];
    ASSERT_EQ(ustr.length() - 2, ustr.operator_index_iterations);
}

TEST(UTF8, operator_index_last_index_increments)
{
    UTF8 ustr = UTF8("Rąbać");

    for (uint32_t i = 1; i < ustr.length(); i++) {
        ustr[i];
        ASSERT_EQ(1, ustr.operator_index_iterations);
    }
}

TEST(UTF8, operator_index_last_index_counting)
{
    UTF8 ustr = UTF8("Rąbać");

    // first element - operator_index_iterations should be equal 0
    ustr[0];
    ASSERT_EQ(0, ustr.operator_index_iterations);
    // third element - operator_index_iterations should be equal 2
    ustr[2];
    ASSERT_EQ(2, ustr.operator_index_iterations);
    // first element - operator_index_iterations should be equal 0
    ustr[0];
    ASSERT_EQ(0, ustr.operator_index_iterations);
    // last element - operator_index_iterations should be equal ustr.strLength - 1
    ustr[ustr.length() - 1];
    ASSERT_EQ(ustr.length() - 1, ustr.operator_index_iterations);
}

TEST(UTF8, substr_returns_empty_string_when_zero_length_is_passed)
{
    UTF8 sourceString = UTF8("Rąbać drewno siekierą");

    UTF8 destinationString = sourceString.substr(1, 0);

    ASSERT_EQ(destinationString.length(), 0);
}

TEST(UTF8, substr_returns_empty_string_when_length_is_exceeded)
{
    UTF8 sourceString = UTF8("Rąbać drewno siekierą");

    uint32_t length        = sourceString.length() + 5;
    UTF8 destinationString = sourceString.substr(1, length);

    ASSERT_EQ(destinationString.length(), 0);
}

TEST(UTF8, substr_returns_proper_begin_char)
{
    UTF8 sourceString      = UTF8("Rąbać drewno siekierą");
    UTF8 destinationString = sourceString.substr(0, 5);

    ASSERT_EQ(destinationString[0], sourceString[0]);
}

TEST(UTF8, substr_returns_proper_string_length)
{
    UTF8 sourceString = UTF8("Rąbać drewno siekierą");
    uint32_t length   = sourceString.length();
    uint32_t position = 0;
    while (length > 1) {
        UTF8 destinationString = sourceString.substr(position, length);
        ASSERT_EQ(destinationString.length(), length);
        position++;
        length--;
    }
}

TEST(UTF8, find_returns_npos_if_not_found)
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "E";
    ASSERT_EQ(-1, sourceString.find(to_find));
}

TEST(UTF8, find_returns_npos_if_pos_exceeds_string_length)
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "A";
    ASSERT_EQ(-1, sourceString.find(to_find, sourceString.length() + 5));
}

TEST(UTF8, find_returns_position_of_passed_string)
{
    std::string base("AaBbCcŃń");
    UTF8 sourceString = UTF8("AaBbCcŃń");

    const char *to_find   = "Ń";
    int32_t base_position = base.find(to_find);

    ASSERT_EQ(base_position, sourceString.find(to_find));
}

TEST(UTF8, find_returns_position_of_passed_string_when_pos_is_passed)
{
    std::string base("Aa Bb aCcŃń");
    UTF8 sourceString = UTF8("Aa Bb aCcŃń");

    const char *to_find   = "a";
    int32_t base_position = base.find(to_find, 3);

    ASSERT_EQ(base_position, sourceString.find(to_find, 3));
}

TEST(UTF8, findLast_returns_npos_if_not_found)
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "E";
    ASSERT_EQ(-1, sourceString.findLast(to_find, sourceString.length() - 1));
}

TEST(UTF8, findLast_returns_npos_if_pos_exceeds_string_length)
{
    UTF8 sourceString   = UTF8("AaBbCcŃń");
    const char *to_find = "A";
    ASSERT_EQ(-1, sourceString.findLast(to_find, sourceString.length() + 5));
}

TEST(UTF8, findLast_returns_position_of_passed_string)
{
    std::string base("AaBbCcŃń");
    UTF8 sourceString = UTF8("AaBbCcŃń");

    const char *to_find   = "Ń";
    int32_t base_position = base.find(to_find);

    ASSERT_EQ(base_position, sourceString.findLast(to_find, sourceString.length() - 1));
}

TEST(UTF8, findLast_returns_position_of_passed_string_when_pos_is_passed)
{
    std::string base("Aa Bb aCcŃń");
    UTF8 sourceString = UTF8("Aa Bb aCcŃń");

    const char *to_find   = "a";
    int32_t base_position = 1;

    ASSERT_EQ(base_position, sourceString.findLast(to_find, 3));
}

TEST(UTF8, split_returns_empty_object_if_idx_exceeds_string_length)
{
    UTF8 sourceString("String testowy PODZIAŁ string testowy");

    UTF8 destinationString = sourceString.split(sourceString.length() + 10);
    ASSERT_EQ(0, destinationString.length());
}

TEST(UTF8, split_returns_propper_string)
{
    UTF8 sourceString("String testowy PODZIAŁ string testowy");
    UTF8 expected("PODZIAŁ string testowy");

    int32_t divisionIdx    = sourceString.find("PODZIAŁ");
    UTF8 destinationString = sourceString.split(divisionIdx);
    ASSERT_STREQ(expected.c_str(), destinationString.c_str());
}

TEST(UTF8, split_strings_have_propper_length_after_split)
{
    UTF8 sourceString("String testowy Ąą Ćć ńźżę PODZIAŁ string testowy Łłódź ");
    UTF8 expectedSource("String testowy Ąą Ćć ńźżę ");
    UTF8 expectedDestination("PODZIAŁ string testowy Łłódź ");

    int32_t divisionIdx = sourceString.find("PODZIAŁ");

    UTF8 destinationString = sourceString.split(divisionIdx);

    ASSERT_EQ(expectedSource.length(), sourceString.length());
    ASSERT_EQ(expectedDestination.length(), destinationString.length());
}

TEST(UTF8, split_summary_length_is_equal_after_split)
{
    UTF8 sourceString("String testowy Ąą Ćć ńźżę PODZIAŁ string testowy Łłódź ");
    int32_t divisionIdx = sourceString.find("PODZIAŁ");

    uint32_t lengthBeforeSplit = sourceString.length();

    UTF8 destinationString = sourceString.split(divisionIdx);

    uint32_t destinationStringLength = destinationString.length();
    uint32_t sourceStringLesngth     = sourceString.length();

    ASSERT_EQ(lengthBeforeSplit, destinationStringLength + sourceStringLesngth);
}

TEST(UTF8, getLine_returns_empty_string_if_its_not_line)
{
    UTF8 sourceString("ĄŚĆćśą Pierwsza linia Druga linia ");

    UTF8 destinationString = sourceString.getLine();

    ASSERT_STREQ(destinationString.c_str(), "");
    ASSERT_EQ(destinationString.length(), 0);
}

TEST(UTF8, getLine_returns_proper_string_when_line_ends_with_n)
{
    UTF8 sourceString("ĄŚĆćśą Pierwsza linia\n Druga linia ");

    UTF8 destinationString = sourceString.getLine();

    ASSERT_STREQ(destinationString.c_str(), "ĄŚĆćśą Pierwsza linia");
}

TEST(UTF8, getLine_returns_proper_string_when_line_ends_with_r)
{
    UTF8 sourceString("ĄŚĆćśą Pierwsza linia\r Druga linia ");

    UTF8 destinationString = sourceString.getLine();

    ASSERT_STREQ(destinationString.c_str(), "ĄŚĆćśą Pierwsza linia");
}

TEST(UTF8, removeChar_returns_false_when_string_to_remove_exceed)
{
    UTF8 sourceString("Teścik");

    bool retVal = sourceString.removeChar(2, sourceString.length() + 1);
    ASSERT_EQ(false, retVal);
}

TEST(UTF8, removeChar_returns_false_when_pos_to_remove_exceed)
{
    UTF8 sourceString("Teścik");

    bool retVal = sourceString.removeChar(sourceString.length() + 1, 2);
    ASSERT_EQ(false, retVal);
}

TEST(UTF8, removeChar_returns_propper_string)
{
    UTF8 sourceString("Teścik");
    UTF8 toCompare("Tecik");
    // remove 'ś'
    bool retVal = sourceString.removeChar(2, 1);
    ASSERT_EQ(true, retVal);
    ASSERT_EQ(toCompare.length(), sourceString.length());
    ASSERT_EQ(toCompare.used(), sourceString.used());
}

TEST(UTF8, removeChar_)
{}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
