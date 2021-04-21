// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <module-audio/Audio/VolumeScaler.hpp>

SCENARIO("Scale volume levels between system and bluetooth")
{
    GIVEN("AVRCP volume")
    {
        WHEN("Volume is 127")
        {
            THEN("System volume is set to 10")
            {
                REQUIRE(audio::volume::scaler::toSystemVolume(127) == 10);
            }
        }
        WHEN("Volume is 100")
        {
            THEN("System volume is set to 7")
            {
                REQUIRE(audio::volume::scaler::toSystemVolume(100) == 7);
            }
        }
        WHEN("Volume is 89")
        {
            THEN("System volume is set to 7")
            {
                REQUIRE(audio::volume::scaler::toSystemVolume(89) == 7);
            }
        }
        WHEN("Volume is 88")
        {
            THEN("System volume is set to 6")
            {
                REQUIRE(audio::volume::scaler::toSystemVolume(88) == 6);
            }
        }
        WHEN("Volume is 13")
        {
            THEN("System volume is set to 1")
            {
                REQUIRE(audio::volume::scaler::toSystemVolume(13) == 1);
            }
        }
        WHEN("Volume is 12")
        {
            THEN("System volume is set to 1")
            {
                REQUIRE(audio::volume::scaler::toSystemVolume(12) == 1);
            }
        }
        WHEN("Volume is 0")
        {
            THEN("System volume is set to 0")
            {
                REQUIRE(audio::volume::scaler::toSystemVolume(0) == 0);
            }
        }
    }

    GIVEN("System volume")
    {
        WHEN("Volume is set to 10")
        {
            THEN("AVRCP volume is 127")
            {
                REQUIRE(audio::volume::scaler::toAvrcpVolume(10) == std::uint8_t{127});
            }
        }
        WHEN("System volume is set to 7")
        {
            THEN("AVRCP volume is 89")
            {
                REQUIRE(audio::volume::scaler::toAvrcpVolume(7) == std::uint8_t{89});
            }
        }
        WHEN("System volume is set to 1")
        {
            THEN("AVRCP volume is 13")
            {
                REQUIRE(audio::volume::scaler::toAvrcpVolume(1) == std::uint8_t{13});
            }
        }
    }
}
