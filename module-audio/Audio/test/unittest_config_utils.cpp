// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <Audio/Profiles/ProfileConfigUtils.hpp>
#include <purefs/filesystem_paths.hpp>

TEST_CASE("QFilterCoefficients - overloaded operator test")
{
    audio::equalizer::QFilterCoefficients coeffs1{1, 1, 1, 1, 1}, coeffs2{1, 1, 1, 1, 1};

    SECTION("Equals")
    {
        REQUIRE(coeffs1 == coeffs2);
    }

    SECTION("Differs 1/5")
    {
        coeffs1.b0 = 2;
        REQUIRE(coeffs1 != coeffs2);
    }

    SECTION("Differs 2/5")
    {
        coeffs2.b1 = 2;
        REQUIRE(coeffs1 != coeffs2);
    }

    SECTION("Differs 3/5")
    {
        coeffs1.a1 = 2;
        REQUIRE(coeffs1 != coeffs2);
    }

    SECTION("Differs 4/5")
    {
        coeffs2.a2 = 2;
        REQUIRE(coeffs1 != coeffs2);
    }

    SECTION("Differs 5/5")
    {
        coeffs2.b2 = 2;
        REQUIRE(coeffs1 != coeffs2);
    }
}

TEST_CASE("Audio profile config utils")
{
    SECTION("Loading config from json file")
    {
        auto config = audio::loadConfigurationFromFile("testfiles/testProfile.json");

        REQUIRE(config.sampleRate_Hz == 44100);
        REQUIRE(config.bitWidth == 8);
        REQUIRE(config.flags == 1);
        REQUIRE(config.inputGain == 2.0);
        REQUIRE(config.outputVolume == 1.0);
        REQUIRE(config.inputPath == audio::codec::InputPath::None);
        REQUIRE(config.outputPath == audio::codec::OutputPath::None);

        audio::equalizer::Equalizer filterCoefficients = {
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::None, 1000.2f, 44100, 0.7f, 10),
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::HighPass, 2000.0f, 8000, 1.7f, -10),
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::LowPass, 10000.0f, 44100, 0.75f, 2.5),
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::Notch, 2500.0f, 44100, 4.4f, 5.3),
            qfilter_CalculateCoeffs(audio::equalizer::FilterType::BandPass, 1000, 44100, 0.7f, 10)};

        for (size_t i = 0; i < audio::equalizer::bands; i++) {
            REQUIRE(config.filterCoefficients.at(i) == filterCoefficients.at(i));
        }
    }
}
