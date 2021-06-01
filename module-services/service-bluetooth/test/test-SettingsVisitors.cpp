// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <SettingsHolder.hpp>

TEST_CASE("Settings Visitors")
{

    SECTION("StringVisitor")
    {
        auto s = bluetooth::StringVisitor();

        SECTION("bool")
        {
            REQUIRE(s(true) == bluetooth::trueStr);
            REQUIRE(s(false) == bluetooth::falseStr);
        }

        SECTION("string")
        {
            REQUIRE(s("true") == bluetooth::trueStr);
        }

        SECTION("int")
        {
            REQUIRE(s(1) == "1");
            REQUIRE(s(-1) == "-1");
        }
    }

    SECTION("IntVisitor")
    {
        auto i          = bluetooth::IntVisitor();
        const char *lol = nullptr;
        SECTION("string")
        {
            CHECK_THROWS_AS(i(lol), std::runtime_error);
            REQUIRE(i("1000") != 1);
            REQUIRE(i("1000") == 1000);
            REQUIRE(i("-1000") == -1000);
            REQUIRE(i("10.00") == 10);
            REQUIRE(i("10tuByłem00") == 10);

            CHECK_THROWS(i("1000000000000000000000"));
        }

        SECTION("bool")
        {
            REQUIRE(i(bool(1000)) == 1);
            REQUIRE(i(bool(0)) == 0);
            REQUIRE(i(true) == 1);
        }

        SECTION("int")
        {
            REQUIRE(i(1000) == 1000);
            REQUIRE(i(-1000) == -1000);
        }
    }

    SECTION("BoolVisitor")
    {
        auto b          = bluetooth::BoolVisitor();
        const char *lol = nullptr;
        SECTION("string")
        {
            CHECK_THROWS(b(lol));
            REQUIRE(b("000") == false);
            REQUIRE(b("0") == false);

            /// TODO is it intended?
            REQUIRE(b("1") == true);
            REQUIRE(b("010") == true);
            REQUIRE(b(1) == true);

            REQUIRE(b("bluetooth::trueStr") == false);

            REQUIRE(b(bluetooth::trueStr) == true);
            REQUIRE(b(bluetooth::falseStr) == false);
        }
    }
}
