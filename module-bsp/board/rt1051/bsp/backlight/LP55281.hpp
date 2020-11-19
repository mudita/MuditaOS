// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace bsp
{
    namespace backlight
    {
        constexpr auto LP55281_DEVICE_ADDR = 0x26;

        // LP55281 registers addresses
        enum class LP55281_Addresses
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
            uint8_t max_current : 2;
            uint8_t current : 6;
        };

        // For specific leds registers
        constexpr auto MAX_DIODE_CURRENT_LIMIT = 0b11000000;
        constexpr auto MAX_BRIGHTNESS_INT      = 63;

        // For BOOST_CTRL register. Agreed to output minimum voltage
        constexpr auto BOOST_OUTPUT_4V = 0x00;

        // For ENABLES register
        constexpr auto NSTDBY       = 0b10000000; // Wakeup from standby
        constexpr auto BOOST_EN     = 0b01000000; // Turn on boost converter
        constexpr auto LED_PORTS_EN = 0b00001110; // Only ports 2-4 active on board
        constexpr auto WAKEUP       = NSTDBY | BOOST_EN | LED_PORTS_EN;

        inline uint8_t encode_diode_brightness(float normalized_brightness)
        {
            // Normalized brightness to 6bit code
            return static_cast<uint8_t>(MAX_BRIGHTNESS_INT * normalized_brightness) & 0b00111111;
        }

    } // namespace backlight
} // namespace bsp