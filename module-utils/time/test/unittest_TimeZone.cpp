// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <time/TimeZone.hpp>
#include <chrono>

TEST_CASE("TimeZone parser")
{
    struct tm summerTime_tm;
    summerTime_tm.tm_year = 121;
    summerTime_tm.tm_mon  = 5;
    summerTime_tm.tm_mday = 20;
    summerTime_tm.tm_hour = 12;
    summerTime_tm.tm_min  = 0;
    summerTime_tm.tm_sec  = 0;

    struct tm winterTime_tm;
    winterTime_tm.tm_year = 121;
    winterTime_tm.tm_mon  = 12;
    winterTime_tm.tm_mday = 25;
    winterTime_tm.tm_hour = 12;
    winterTime_tm.tm_min  = 0;
    winterTime_tm.tm_sec  = 0;

    time_t summerTime = mktime(&summerTime_tm);
    time_t winterTime = mktime(&winterTime_tm);

    SECTION("Checking the time zone rules for Warsaw")
    {
        std::string zone{"Warsaw"};
        auto rules        = utils::time::getTimeZoneRules(zone);
        auto summerOffset = utils::time::getTimeZoneOffset(zone, summerTime);
        auto winterOffset = utils::time::getTimeZoneOffset(zone, winterTime);

        REQUIRE(summerOffset == std::chrono::hours(2));
        REQUIRE(winterOffset == std::chrono::hours(1));
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for London")
    {
        std::string zone{"London"};
        auto rules        = utils::time::getTimeZoneRules(zone);
        auto summerOffset = utils::time::getTimeZoneOffset(zone, summerTime);
        auto winterOffset = utils::time::getTimeZoneOffset(zone, winterTime);

        REQUIRE(summerOffset == std::chrono::hours(1));
        REQUIRE(winterOffset == std::chrono::hours(0));
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for New York")
    {
        std::string zone{"New York"};
        auto rules        = utils::time::getTimeZoneRules(zone);
        auto summerOffset = utils::time::getTimeZoneOffset(zone, summerTime);
        auto winterOffset = utils::time::getTimeZoneOffset(zone, winterTime);

        REQUIRE(summerOffset == std::chrono::hours(-4));
        REQUIRE(winterOffset == std::chrono::hours(-5));
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for Tehran")
    {
        std::string zone{"Tehran"};
        auto rules        = utils::time::getTimeZoneRules(zone);
        auto summerOffset = utils::time::getTimeZoneOffset(zone, summerTime);
        auto winterOffset = utils::time::getTimeZoneOffset(zone, winterTime);

        REQUIRE(summerOffset == std::chrono::hours(4) + std::chrono::minutes(30));
        REQUIRE(winterOffset == std::chrono::hours(3) + std::chrono::minutes(30));
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for Auckland")
    {
        std::string zone{"Auckland"};
        auto rules        = utils::time::getTimeZoneRules(zone);
        auto summerOffset = utils::time::getTimeZoneOffset(zone, summerTime);
        auto winterOffset = utils::time::getTimeZoneOffset(zone, winterTime);

        REQUIRE(summerOffset == std::chrono::hours(12));
        REQUIRE(winterOffset == std::chrono::hours(13));
        REQUIRE(rules.size() != 0);
    }

    SECTION("Checking the time zone rules for unknown zone")
    {
        std::string zone{"unknown"};
        auto rules        = utils::time::getTimeZoneRules(zone);
        auto summerOffset = utils::time::getTimeZoneOffset(zone, summerTime);
        auto winterOffset = utils::time::getTimeZoneOffset(zone, winterTime);

        REQUIRE(summerOffset == std::chrono::hours(0));
        REQUIRE(winterOffset == std::chrono::hours(0));
        REQUIRE(rules.size() == 0);
    }
}
