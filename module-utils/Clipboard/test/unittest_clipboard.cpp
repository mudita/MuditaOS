// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <Clipboard.hpp>

TEST_CASE("Clipboard")
{
    SECTION("No data")
    {
        // It is guaranted to have no data only during 1st call to singleton getInstance()
        REQUIRE(Clipboard::getInstance().gotData() == false);
        REQUIRE(Clipboard::getInstance().paste() == "");
    }

    SECTION("Single copy")
    {
        const std::string test1 = "test1";
        const std::string test2 = "test2";
        Clipboard::getInstance().copy(test1);
        REQUIRE(Clipboard::getInstance().gotData() == true);
        REQUIRE(Clipboard::getInstance().paste() == test1);
        REQUIRE(Clipboard::getInstance().gotData() == true);
    }

    SECTION("Double copy")
    {
        const std::string test1 = "test1";
        const std::string test2 = "test2";
        Clipboard::getInstance().copy(test1);
        REQUIRE(Clipboard::getInstance().gotData() == true);
        Clipboard::getInstance().copy(test2);
        REQUIRE(Clipboard::getInstance().gotData() == true);
        REQUIRE(Clipboard::getInstance().paste() == test2);
        REQUIRE(Clipboard::getInstance().gotData() == true);
    }

    SECTION("Copy empty string")
    {
        const std::string test1 = "";
        Clipboard::getInstance().copy(test1);
        REQUIRE(Clipboard::getInstance().gotData() == true);
        REQUIRE(Clipboard::getInstance().paste() == test1);
        REQUIRE(Clipboard::getInstance().gotData() == true);
    }
}
