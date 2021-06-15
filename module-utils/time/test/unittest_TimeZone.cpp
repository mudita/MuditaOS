// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <time/TimeZone.hpp>

TEST_CASE("TimeZone parser")
{
    SECTION("Checking the time zone rules for Warsaw")
    {
        std::string zone{"Warsaw"};
        auto rules  = utils::time::getTimeZoneRules(zone);
        auto offset = utils::time::getTimeZoneOffset(zone);

        REQUIRE(offset == 4);
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for London")
    {
        std::string zone{"London"};
        auto rules  = utils::time::getTimeZoneRules(zone);
        auto offset = utils::time::getTimeZoneOffset(zone);

        REQUIRE(offset == 0);
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for New York")
    {
        std::string zone{"New York"};
        auto rules  = utils::time::getTimeZoneRules(zone);
        auto offset = utils::time::getTimeZoneOffset(zone);

        REQUIRE(offset == -20);
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for Tehran")
    {
        std::string zone{"Tehran"};
        auto rules  = utils::time::getTimeZoneRules(zone);
        auto offset = utils::time::getTimeZoneOffset(zone);

        REQUIRE(offset == 14);
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for Auckland")
    {
        std::string zone{"Auckland"};
        auto rules  = utils::time::getTimeZoneRules(zone);
        auto offset = utils::time::getTimeZoneOffset(zone);

        REQUIRE(offset == 48);
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for unknown zone")
    {
        std::string zone{"unknown"};
        auto rules  = utils::time::getTimeZoneRules(zone);
        auto offset = utils::time::getTimeZoneOffset(zone);

        REQUIRE(offset == 0);
        REQUIRE(rules.size() == 0);
    }
}
