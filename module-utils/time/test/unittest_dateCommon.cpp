// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <time/dateCommon.hpp>

TEST_CASE("Get date after given time")
{
    auto givenTime   = TimePointFromString("2022-11-11 11:00:00");
    auto timeBefore1 = TimePointFromString("2022-11-11 10:59:01");
    auto timeBefore2 = TimePointFromString("2022-11-05 10:59:01");
    auto timeBefore3 = TimePointFromString("2022-11-05 15:59:01");
    auto timeBefore4 = TimePointFromString("1999-01-05 10:59:01");

    auto timeAfter1 = TimePointFromString("2022-11-11 11:00:01");
    auto timeAfter2 = TimePointFromString("2022-11-12 11:00:01");
    auto timeAfter3 = TimePointFromString("2022-11-12 10:59:01");
    auto timeAfter4 = TimePointFromString("2222-11-12 11:59:01");

    SECTION("Time Before")
    {
        auto result = GetFollowingDayTime(givenTime, givenTime);
        REQUIRE(result == TimePointFromString("2022-11-12 11:00:00"));
        result = GetFollowingDayTime(timeBefore1, givenTime);
        REQUIRE(result == TimePointFromString("2022-11-12 10:59:01"));
        result = GetFollowingDayTime(timeBefore2, givenTime);
        REQUIRE(result == TimePointFromString("2022-11-12 10:59:01"));
        result = GetFollowingDayTime(timeBefore3, givenTime);
        REQUIRE(result == TimePointFromString("2022-11-11 15:59:01"));
        result = GetFollowingDayTime(timeBefore4, givenTime);
        REQUIRE(result == TimePointFromString("2022-11-12 10:59:01"));
    }

    SECTION("Time After")
    {
        auto result = GetFollowingDayTime(timeAfter1, givenTime);
        REQUIRE(result == timeAfter1);
        result = GetFollowingDayTime(timeAfter2, givenTime);
        REQUIRE(result == TimePointFromString("2022-11-11 11:00:01"));
        result = GetFollowingDayTime(timeAfter3, givenTime);
        REQUIRE(result == timeAfter3);
        result = GetFollowingDayTime(timeAfter4, givenTime);
        REQUIRE(result == TimePointFromString("2022-11-11 11:59:01"));
    }
}
