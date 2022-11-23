// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <i18n/i18n.hpp>
#include <string>

#include <purefs/filesystem_paths.hpp>

using namespace std;

TEST_CASE("Test set display language - empty display language string")
{
    utils::resetAssetsPath(purefs::dir::getSystemDataDirPath());
    utils::resetDisplayLanguages();
    REQUIRE(utils::getDisplayLanguage().empty());

    // Invalid string provided - language should be set to default one.
    CHECK_FALSE(utils::setDisplayLanguage(""));
    REQUIRE(utils::getDisplayLanguage() == utils::getDefaultLanguage());
}

TEST_CASE("Test set display language - invalid display language string")
{
    utils::resetAssetsPath(purefs::dir::getSystemDataDirPath());
    utils::resetDisplayLanguages();
    REQUIRE(utils::getDisplayLanguage().empty());

    // Invalid string provided - language should be set to default one.
    CHECK_FALSE(utils::setDisplayLanguage("NonExistingLanguage"));
    REQUIRE(utils::getDisplayLanguage() == utils::getDefaultLanguage());
}

TEST_CASE("Test set display language - double invalid display language string")
{
    utils::resetAssetsPath(purefs::dir::getSystemDataDirPath());
    utils::resetDisplayLanguages();
    REQUIRE(utils::getDisplayLanguage().empty());

    // Invalid string provided - language should be set to default one.
    CHECK_FALSE(utils::setDisplayLanguage(""));
    REQUIRE(utils::getDisplayLanguage() == utils::getDefaultLanguage());

    // Invalid string provided - language already set to default one so method returns false.
    CHECK_FALSE(utils::setDisplayLanguage("NonExistingLanguage"));
    REQUIRE(utils::getDisplayLanguage() == utils::getDefaultLanguage());
}

TEST_CASE("Test set display language - set display language with valid string")
{
    static constexpr auto languageToChange = "Polski";

    utils::resetAssetsPath(purefs::dir::getSystemDataDirPath());
    utils::resetDisplayLanguages();
    REQUIRE(utils::getDisplayLanguage().empty());

    // Valid string provided - language should change.
    CHECK(utils::setDisplayLanguage(languageToChange));
    REQUIRE(utils::getDisplayLanguage() == languageToChange);
}

TEST_CASE("Test set display language - set display language with valid string and try to invalidate")
{
    static constexpr auto languageToChange = "Polski";

    utils::resetAssetsPath(purefs::dir::getSystemDataDirPath());
    utils::resetDisplayLanguages();
    REQUIRE(utils::getDisplayLanguage().empty());

    // Valid string provided - language should change.
    CHECK(utils::setDisplayLanguage(languageToChange));
    REQUIRE(utils::getDisplayLanguage() == languageToChange);

    // Invalid string provided - language should not be changed as valid one is set.
    CHECK_FALSE(utils::setDisplayLanguage(""));
    REQUIRE(utils::getDisplayLanguage() == languageToChange);
}

TEST_CASE("Test get string method - no display language set")
{
    utils::resetAssetsPath(purefs::dir::getSystemDataDirPath());
    utils::resetDisplayLanguages();
    REQUIRE(utils::getDisplayLanguage().empty());

    // No languages provided
    REQUIRE(utils::translate("common_yes") == "common_yes");
}

TEST_CASE("Test get string method - invalid display language set")
{
    utils::resetAssetsPath(purefs::dir::getSystemDataDirPath());
    utils::resetDisplayLanguages();
    REQUIRE(utils::getDisplayLanguage().empty());

    // Invalid string provided - language should be set to default one.
    CHECK_FALSE(utils::setDisplayLanguage(""));
    REQUIRE(utils::getDisplayLanguage() == utils::getDefaultLanguage());

    // Default fallback language set - English
    REQUIRE(utils::translate("common_yes") == "Yes");
}

TEST_CASE("Test get string method - valid display language set")
{
    static constexpr auto languageToChange = "Polski";

    utils::resetAssetsPath(purefs::dir::getSystemDataDirPath());
    utils::resetDisplayLanguages();
    REQUIRE(utils::getDisplayLanguage().empty());

    // Valid string provided - language should change.
    CHECK(utils::setDisplayLanguage(languageToChange));
    REQUIRE(utils::getDisplayLanguage() == languageToChange);

    // Display language set - Polish
    REQUIRE(utils::translate("common_yes") == "Tak");
}
