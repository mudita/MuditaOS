// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <Math.hpp>

#include <climits>

using namespace trigonometry;
using namespace binary;

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

    SECTION("Binary operations")
    {
        SECTION("Is number a power of two")
        {
            REQUIRE(isPowerOfTwo(0) == false);
            REQUIRE(isPowerOfTwo(1) == true);
            REQUIRE(isPowerOfTwo(2) == true);
            REQUIRE(isPowerOfTwo(3) == false);
            REQUIRE(isPowerOfTwo(64) == true);
            REQUIRE(isPowerOfTwo(128) == true);
            REQUIRE(isPowerOfTwo(64 + 128) == false);
        }

        SECTION("Find nearest power of two (floor)")
        {
            REQUIRE(floorPowerOfTwo(0) == 0);
            REQUIRE(floorPowerOfTwo(1) == 1);
            REQUIRE(floorPowerOfTwo(2) == 2);
            REQUIRE(floorPowerOfTwo(3) == 2);
            REQUIRE(floorPowerOfTwo(7) == 4);
            REQUIRE(floorPowerOfTwo(0xffffffff) == 0x80000000);
        }

        SECTION("Find nearest power of two (ceil)")
        {
            REQUIRE(ceilPowerOfTwo(0) == 0);
            REQUIRE(ceilPowerOfTwo(1) == 1);
            REQUIRE(ceilPowerOfTwo(2) == 2);
            REQUIRE(ceilPowerOfTwo(3) == 4);
            REQUIRE(ceilPowerOfTwo(7) == 8);
            REQUIRE(ceilPowerOfTwo(62) == 64);
            REQUIRE(ceilPowerOfTwo(UINT_MAX) == 0);
        }
    }
}
