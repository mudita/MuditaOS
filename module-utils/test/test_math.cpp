// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include <catch2/catch.hpp>

#include "math/Math.hpp"

using namespace trigonometry;

TEST_CASE("Math")
{
    SECTION("Trigonometry")
    {
        SECTION("Degrees to radians")
        {
            REQUIRE(toRadians(-720) == Approx(-4 * M_PI));
            REQUIRE(toRadians(-360) == Approx(-2 * M_PI));
            REQUIRE(toRadians(-180) == Approx(-M_PI));
            REQUIRE(toRadians(-30) == Approx(-M_PI / 6));
            REQUIRE(toRadians(-12) == Approx(-M_PI / 15));
            REQUIRE(toRadians(-1) == Approx(-M_PI / 180));
            REQUIRE(toRadians(1) == Approx(M_PI / 180));
            REQUIRE(toRadians(12) == Approx(M_PI / 15));
            REQUIRE(toRadians(30) == Approx(M_PI / 6));
            REQUIRE(toRadians(180) == Approx(M_PI));
            REQUIRE(toRadians(360) == Approx(2 * M_PI));
            REQUIRE(toRadians(720) == Approx(4 * M_PI));
        }

        SECTION("Adjacent side length calculation")
        {
            REQUIRE(AdjacentSide::fromAngle(toRadians(30), 6) == 5);
            REQUIRE(AdjacentSide::fromCosine(std::cos(toRadians(30)), 6) == 5);

            REQUIRE(AdjacentSide::fromAngle(toRadians(60), 6) == 3);
            REQUIRE(AdjacentSide::fromCosine(std::cos(toRadians(60)), 6) == 3);
        }

        SECTION("Opposite side length calculation")
        {
            REQUIRE(OppositeSide::fromAngle(toRadians(30), 6) == 3);
            REQUIRE(OppositeSide::fromSine(std::sin(toRadians(30)), 6) == 3);

            REQUIRE(OppositeSide::fromAngle(toRadians(60), 6) == 5);
            REQUIRE(OppositeSide::fromSine(std::sin(toRadians(60)), 6) == 5);
        }
    }
}
