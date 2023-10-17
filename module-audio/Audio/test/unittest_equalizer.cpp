// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <module-audio/Audio/equalizer/Equalizer.hpp>
#include <Utils.hpp>
#include <stdexcept>

SCENARIO("Calculate filter coeff")
{
    using namespace audio::equalizer;

    GIVEN("High pass filter")
    {
        const auto filterHighPass = qfilter_CalculateCoeffs(FilterType::HighPass, 300.9f, 44100, 0.701f, 0);
        THEN("Registers 1,2,3 should match b0 setup")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterHighPass.b0);
            REQUIRE(byte1 == 0x0F);
            REQUIRE(byte2 == 0x84);
            REQUIRE(byte3 == 0xAD);
        }
        THEN("Registers 4,5,6 should match b1 setup")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterHighPass.b1);
            REQUIRE(byte1 == 0xE0);
            REQUIRE(byte2 == 0xF6);
            REQUIRE(byte3 == 0xA6);
        }
        THEN("Registers 7,8,9 should match b2 setup")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterHighPass.b2);
            REQUIRE(byte1 == 0x0F);
            REQUIRE(byte2 == 0x84);
            REQUIRE(byte3 == 0xAD);
        }
        THEN("Registers 10,11,12 should match a1 setup")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterHighPass.a1);
            REQUIRE(byte1 == 0xE0);
            REQUIRE(byte2 == 0xFA);
            REQUIRE(byte3 == 0x4D);
        }
        THEN("Registers 13,14,15 should match a2 setup")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterHighPass.a2);
            REQUIRE(byte1 == 0x0F);
            REQUIRE(byte2 == 0x0D);
            REQUIRE(byte3 == 0x01);
        }
    }
    GIVEN("High shelf filter")
    {
        const auto filterHighShelf = qfilter_CalculateCoeffs(FilterType::LowShelf, 401.f, 44100, 0.701f, -10);
        THEN("Registers 1,2 should match b0 setup")
        {
            const auto [byte1, byte2, _] = utils::floatingPointConverter(filterHighShelf.b0);
            REQUIRE(byte1 == 0x0F);
            REQUIRE(byte2 == 0x9F);
        }
        THEN("Registers 4,5 should match b1 setup")
        {
            const auto [byte1, byte2, _] = utils::floatingPointConverter(filterHighShelf.b1);
            REQUIRE(byte1 == 0xE1);
            REQUIRE(byte2 == 0xB4);
        }
        THEN("Registers 7,8 should match b2 setup")
        {
            const auto [byte1, byte2, _] = utils::floatingPointConverter(filterHighShelf.b2);
            REQUIRE(byte1 == 0x0E);
            REQUIRE(byte2 == 0xB2);
        }
        THEN("Registers 10,11 should match a1 setup")
        {
            const auto [byte1, byte2, _] = utils::floatingPointConverter(filterHighShelf.a1);
            REQUIRE(byte1 == 0xE1);
            REQUIRE(byte2 == 0xBC);
        }
        THEN("Registers 13,14 should match a2 setup")
        {
            const auto [byte1, byte2, _] = utils::floatingPointConverter(filterHighShelf.a2);
            REQUIRE(byte1 == 0x0E);
            REQUIRE(byte2 == 0x5A);
        }
    }

    GIVEN("Filter none")
    {
        const auto filterNone = qfilter_CalculateCoeffs(FilterType::None, 0, 0, 0, 0);
        THEN("Register 1 should be equal to 16. Registers 2,3 should be equal to 0")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterNone.b0);
            REQUIRE(byte1 == 0x10);
            REQUIRE(byte2 == 0x00);
            REQUIRE(byte3 == 0x00);
        }
        THEN("Registers 4,5,6 should be equal to 0")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterNone.b1);
            REQUIRE(byte1 == 0x00);
            REQUIRE(byte2 == 0x00);
            REQUIRE(byte3 == 0x00);
        }
        THEN("Registers 7,8,9 should be equal to 0")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterNone.b2);
            REQUIRE(byte1 == 0x00);
            REQUIRE(byte2 == 0x00);
            REQUIRE(byte3 == 0x00);
        }
        THEN("Registers 10,11,12 should be equal to 0")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterNone.a1);
            REQUIRE(byte1 == 0x00);
            REQUIRE(byte2 == 0x00);
            REQUIRE(byte3 == 0x00);
        }
        THEN("Registers 13,14,15 should be equal to 0")
        {
            const auto [byte1, byte2, byte3] = utils::floatingPointConverter(filterNone.a2);
            REQUIRE(byte1 == 0x00);
            REQUIRE(byte2 == 0x00);
            REQUIRE(byte3 == 0x00);
        }
    }

    GIVEN("Filter with Q out of range")
    {
        WHEN("Q is too low")
        {
            THEN("Calculation of coefficients should throw")
            {
                REQUIRE_THROWS_AS(qfilter_CalculateCoeffs(FilterType::HighPass, 300.9f, 44100, -1.f, 0),
                                  std::invalid_argument);
            }
        }
        WHEN("Q is too high")
        {
            THEN("Calculation of coefficients should throw")
            {
                REQUIRE_THROWS_AS(qfilter_CalculateCoeffs(FilterType::HighPass, 300.9f, 44100, 100.f, 0),
                                  std::invalid_argument);
            }
        }
    }

    GIVEN("Filter with negative frequency")
    {
        THEN("Calculation of coefficients should throw")
        {
            REQUIRE_THROWS_AS(qfilter_CalculateCoeffs(FilterType::HighPass, -300.9f, 44100, 0.2f, 0),
                              std::invalid_argument);
        }
    }

    GIVEN("Filter with sample rate equal to 0")
    {
        THEN("Calculation of coefficients should throw")
        {
            REQUIRE_THROWS_AS(qfilter_CalculateCoeffs(FilterType::HighPass, 300.9f, 0, 0.2f, 0), std::invalid_argument);
        }
    }
}
