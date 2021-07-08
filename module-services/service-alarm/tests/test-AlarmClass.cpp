// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <service-alarm/AlarmObject.hpp>

#include <chrono>

TEST_CASE("Service Alarm - class alarm")
{
    using namespace std::chrono_literals;
    AlarmTimeStamp now = std::chrono::system_clock::now();

    SECTION("Assigment operator")
    {
        AlarmObject alarmFirst(now);
        AlarmObject alarmSecond;
        alarmSecond = alarmFirst;
        REQUIRE(alarmFirst == alarmSecond);
    }

    SECTION("Comparison operators")
    {
        AlarmObject alarmFirst(now);
        AlarmObject alarmSecond(now + 1h);
        AlarmObject alarmThird(now + 1h);

        REQUIRE_FALSE(alarmFirst == alarmSecond);
        REQUIRE(alarmFirst != alarmSecond);
        REQUIRE_FALSE(alarmSecond != alarmThird);

        REQUIRE(alarmFirst <= alarmSecond);
        REQUIRE(alarmFirst < alarmSecond);

        REQUIRE_FALSE(alarmFirst > alarmSecond);
        REQUIRE_FALSE(alarmFirst >= alarmSecond);

        REQUIRE(alarmSecond <= alarmThird);
        REQUIRE(alarmSecond >= alarmThird);
        REQUIRE_FALSE(alarmSecond < alarmThird);
        REQUIRE_FALSE(alarmSecond > alarmThird);
    }
}
