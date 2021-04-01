// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <i18n/i18n.hpp>
#include <string>

using namespace std;

TEST_CASE("Test set display language - empty display language string")
{
    utils::localize.resetDisplayLanguages();
    REQUIRE(utils::localize.getDisplayLanguage() == "");

    // Invalid string provided - language should be set to default one.
    CHECK(utils::localize.setDisplayLanguage(""));
    REQUIRE(utils::localize.getDisplayLanguage() == utils::localize.DefaultLanguage);
}

TEST_CASE("Test set display language - invalid display language string")
{
    utils::localize.resetDisplayLanguages();
    REQUIRE(utils::localize.getDisplayLanguage() == "");

    // Invalid string provided - language should be set to default one.
    CHECK(utils::localize.setDisplayLanguage("NonExistingLanguage"));
    REQUIRE(utils::localize.getDisplayLanguage() == utils::localize.DefaultLanguage);
}

TEST_CASE("Test set display language - double invalid display language string")
{
    utils::localize.resetDisplayLanguages();
    REQUIRE(utils::localize.getDisplayLanguage() == "");

    // Invalid string provided - language should be set to default one.
    CHECK(utils::localize.setDisplayLanguage(""));
    REQUIRE(utils::localize.getDisplayLanguage() == utils::localize.DefaultLanguage);

    // Invalid string provided - language already set to default one so method returns false.
    CHECK_FALSE(utils::localize.setDisplayLanguage("NonExistingLanguage"));
    REQUIRE(utils::localize.getDisplayLanguage() == utils::localize.DefaultLanguage);
}

TEST_CASE("Test set display language - set display language with valid string")
{
    static constexpr auto languageToChange = "Polski";

    utils::localize.resetDisplayLanguages();
    REQUIRE(utils::localize.getDisplayLanguage() == "");

    // Valid string provided - language should change.
    CHECK(utils::localize.setDisplayLanguage(languageToChange));
    REQUIRE(utils::localize.getDisplayLanguage() == languageToChange);
}

TEST_CASE("Test set display language - set display language with valid string and try to invalidate")
{
    static constexpr auto languageToChange = "Polski";

    utils::localize.resetDisplayLanguages();
    REQUIRE(utils::localize.getDisplayLanguage() == "");

    // Valid string provided - language should change.
    CHECK(utils::localize.setDisplayLanguage(languageToChange));
    REQUIRE(utils::localize.getDisplayLanguage() == languageToChange);

    // Invalid string provided - language should not be changed as valid one is set.
    CHECK_FALSE(utils::localize.setDisplayLanguage(""));
    REQUIRE(utils::localize.getDisplayLanguage() == languageToChange);
}

TEST_CASE("Test get string method - no display language set")
{
    utils::localize.resetDisplayLanguages();
    REQUIRE(utils::localize.getDisplayLanguage() == "");

    // No languages provided
    REQUIRE(utils::localize.get("common_yes") == "common_yes");
}

TEST_CASE("Test get string method - invalid display language set")
{
    utils::localize.resetDisplayLanguages();
    REQUIRE(utils::localize.getDisplayLanguage() == "");

    // Invalid string provided - language should be set to default one.
    CHECK(utils::localize.setDisplayLanguage(""));
    REQUIRE(utils::localize.getDisplayLanguage() == utils::localize.DefaultLanguage);

    // Default fallback language set - English
    REQUIRE(utils::localize.get("common_yes") == "Yes");
}

TEST_CASE("Test get string method - valid display language set")
{
    static constexpr auto languageToChange = "Polski";

    utils::localize.resetDisplayLanguages();
    REQUIRE(utils::localize.getDisplayLanguage() == "");

    // Valid string provided - language should change.
    CHECK(utils::localize.setDisplayLanguage(languageToChange));
    REQUIRE(utils::localize.getDisplayLanguage() == languageToChange);

    // Display language set - Polish
    REQUIRE(utils::localize.get("common_yes") == "Tak");
}
