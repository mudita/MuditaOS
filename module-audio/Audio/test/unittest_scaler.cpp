// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
                REQUIRE(audio::volume::scaler::a2dp::toSystemVolume(127) == 10);
            }
        }
        WHEN("Volume is 100")
        {
            THEN("System volume is set to 7")
            {
                REQUIRE(audio::volume::scaler::a2dp::toSystemVolume(100) == 7);
            }
        }
        WHEN("Volume is 89")
        {
            THEN("System volume is set to 7")
            {
                REQUIRE(audio::volume::scaler::a2dp::toSystemVolume(89) == 7);
            }
        }
        WHEN("Volume is 88")
        {
            THEN("System volume is set to 6")
            {
                REQUIRE(audio::volume::scaler::a2dp::toSystemVolume(88) == 6);
            }
        }
        WHEN("Volume is 13")
        {
            THEN("System volume is set to 1")
            {
                REQUIRE(audio::volume::scaler::a2dp::toSystemVolume(13) == 1);
            }
        }
        WHEN("Volume is 12")
        {
            THEN("System volume is set to 1")
            {
                REQUIRE(audio::volume::scaler::a2dp::toSystemVolume(12) == 1);
            }
        }
        WHEN("Volume is 0")
        {
            THEN("System volume is set to 0")
            {
                REQUIRE(audio::volume::scaler::a2dp::toSystemVolume(0) == 0);
            }
        }
    }

    GIVEN("System volume")
    {
        WHEN("Volume is set to 10")
        {
            THEN("AVRCP volume is 127")
            {
                REQUIRE(audio::volume::scaler::a2dp::toAvrcpVolume(10) == std::uint8_t{127});
            }
            THEN("HSP speaker gain is 15")
            {
                REQUIRE(audio::volume::scaler::hsp::toHSPGain(10) == std::uint8_t{15});
            }
        }
        WHEN("System volume is set to 7")
        {
            THEN("AVRCP volume is 89")
            {
                REQUIRE(audio::volume::scaler::a2dp::toAvrcpVolume(7) == std::uint8_t{89});
            }
            THEN("HSP speaker gain is 7")
            {
                REQUIRE(audio::volume::scaler::hsp::toHSPGain(7) == std::uint8_t{11});
            }
        }
        WHEN("System volume is set to 1")
        {
            THEN("AVRCP volume is 13")
            {
                REQUIRE(audio::volume::scaler::a2dp::toAvrcpVolume(1) == std::uint8_t{13});
            }
            THEN("HSP speaker gain is 2")
            {
                REQUIRE(audio::volume::scaler::hsp::toHSPGain(1) == std::uint8_t{2});
            }
        }
    }
    GIVEN("HSP speaker gain")
    {
        WHEN("Gain is 15")
        {
            THEN("System volume is set to 10")
            {
                REQUIRE(audio::volume::scaler::hsp::toSystemVolume(15) == 10);
            }
        }
        WHEN("Gain is 12")
        {
            THEN("System volume is set to 8")
            {
                REQUIRE(audio::volume::scaler::hsp::toSystemVolume(12) == 8);
            }
        }
        WHEN("Gain is 10")
        {
            THEN("System volume is set to 6")
            {
                REQUIRE(audio::volume::scaler::hsp::toSystemVolume(10) == 6);
            }
        }
        WHEN("Gain is 7")
        {
            THEN("System volume is set to 4")
            {
                REQUIRE(audio::volume::scaler::hsp::toSystemVolume(7) == 4);
            }
        }
        WHEN("Gain is 3")
        {
            THEN("System volume is set to 2")
            {
                REQUIRE(audio::volume::scaler::hsp::toSystemVolume(3) == 2);
            }
        }
        WHEN("Gain is 1")
        {
            THEN("System volume is set to 1")
            {
                REQUIRE(audio::volume::scaler::hsp::toSystemVolume(1) == 1);
            }
        }
        WHEN("Volume is 0")
        {
            THEN("System volume is set to 0")
            {
                REQUIRE(audio::volume::scaler::hsp::toSystemVolume(0) == 0);
            }
        }
    }
}
