// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <module-bsp/board/rt1051/puretx/bsp/battery_charger/battery_charger_utils.hpp>

TEST_CASE("BatteryChargerUtilsTest")
{
    SECTION("2s compliment conversion test")
    {
        constexpr std::array uint8Fixtures = {std::make_pair<std::uint8_t, int>(0x00, 0),
                                              std::make_pair<std::uint8_t, int>(0x80, -128),
                                              std::make_pair<std::uint8_t, int>(0xFF, -1),
                                              std::make_pair<std::uint8_t, int>(0x7F, 127)};

        constexpr std::array uint16Fixtures = {std::make_pair<std::uint16_t, int>(0x0000, 0),
                                               std::make_pair<std::uint16_t, int>(0x8000, -32768),
                                               std::make_pair<std::uint16_t, int>(0xFFFF, -1),
                                               std::make_pair<std::uint16_t, int>(0x7FFF, 32767)};

        constexpr std::array uint32Fixtures = {std::make_pair<std::uint32_t, int>(0x00000000, 0),
                                               std::make_pair<std::uint32_t, int>(0x80000000, -2147483648),
                                               std::make_pair<std::uint32_t, int>(0xFFFFFFFF, -1),
                                               std::make_pair<std::uint32_t, int>(0x7FFFFFFF, 2147483647)};

        for (const auto &fixture : uint8Fixtures) {
            CHECK(bsp::battery_charger::utils::twosComplimentToInt(fixture.first) == fixture.second);
        }

        for (const auto &fixture : uint16Fixtures) {
            CHECK(bsp::battery_charger::utils::twosComplimentToInt(fixture.first) == fixture.second);
        }

        for (const auto &fixture : uint32Fixtures) {
            CHECK(bsp::battery_charger::utils::twosComplimentToInt(fixture.first) == fixture.second);
        }
    }
}
