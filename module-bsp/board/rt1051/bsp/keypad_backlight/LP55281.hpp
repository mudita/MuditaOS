// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <algorithm>

namespace bsp::keypad_backlight
{
    constexpr inline auto LP55281_DEVICE_ADDR = 0x4C;

    enum class LP55281_Registers
    {
        RED1       = 0x00,
        GREEN1     = 0x01,
        BLUE1      = 0x02,
        RED2       = 0x03,
        GREEN2     = 0x04,
        BLUE2      = 0x05,
        RED3       = 0x06,
        GREEN3     = 0x07,
        BLUE3      = 0x08,
        RED4       = 0x09,
        GREEN4     = 0x0a,
        BLUE4      = 0x0b,
        ALED       = 0x0c,
        CTRL1      = 0x0d,
        CTRL2      = 0x0e,
        BOOST_CTRL = 0x0f,
        FREQ_SEL   = 0x10,
        ENABLES    = 0x11,
        LED_TEST   = 0x12,
        ADC_OUT    = 0x13,
        RESET      = 0x60,
    };

    struct Diode_Reg
    {
        std::uint8_t max_current : 2;
        std::uint8_t current : 6;
    };

    constexpr inline auto MAX_DIODE_CURRENT_LIMIT = 0b11;
    constexpr inline auto MAX_BRIGHTNESS_INT      = 63;

    constexpr inline auto BOOST_OUTPUT_4V = 0x00;

    constexpr inline auto NSTDBY                 = 0b10000000;
    constexpr inline auto BOOST_EN               = 0b01000000;
    constexpr inline auto FUNCTION_KEYS_PORTS_EN = NSTDBY | BOOST_EN | 0b00000110; // Only ports 2-3 active
    constexpr inline auto NUMERIC_KEYS_PORT_EN   = NSTDBY | BOOST_EN | 0b00001000; // Only port 4 active
    constexpr inline auto ALL_PORTS_EN           = FUNCTION_KEYS_PORTS_EN | NUMERIC_KEYS_PORT_EN; // ports 2-4 active

    constexpr inline auto LED_TEST_THRESHOLD = 3; // 0.18mA = (15mA/255) * 3
    constexpr inline auto EN_LED_TEST        = 0b00010000;

    constexpr std::uint8_t encode_diode_brightness_to_6bits(float normalized_brightness)
    {
        return std::clamp(static_cast<std::uint8_t>(MAX_BRIGHTNESS_INT * normalized_brightness),
                          static_cast<std::uint8_t>(0),
                          static_cast<std::uint8_t>(MAX_BRIGHTNESS_INT));
    }

} // namespace bsp::keypad_backlight
