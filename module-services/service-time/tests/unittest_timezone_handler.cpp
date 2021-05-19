// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <module-services/service-time/service-time/TimezoneHandler.hpp>

#include <catch2/catch.hpp>

#include <string>

TEST_CASE("Not known timezone")
{
    SECTION("Valid timezone negative shift")
    {
        TimezoneHandler timezoneHandler(std::chrono::minutes{120});
        auto validTimezone = timezoneHandler.getTimezone();

        REQUIRE(validTimezone == std::string("MUD-2:00"));
    }

    SECTION("Valid timezone positive shift")
    {
        TimezoneHandler timezoneHandler(std::chrono::minutes{-120});
        auto validTimezone = timezoneHandler.getTimezone();

        REQUIRE(validTimezone == std::string("MUD+2:00"));
    }

    SECTION("Invalid shift")
    {
        TimezoneHandler timezoneHandler(std::chrono::minutes{60 * 12});
        auto invalidTimezone = timezoneHandler.getTimezone();
        REQUIRE(invalidTimezone == std::string("MUD-0:00"));
    }
}
