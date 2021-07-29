// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

namespace bsp::fuel_gauge
{

    constexpr inline auto FUEL_GAUGE_I2C_ADDR = 0x62;
    constexpr inline auto BATTERY_INFO_SIZE   = 64;

    enum class Registers
    {
        VERSION    = 0x00,
        VCELL_H    = 0x02,
        VCELL_L    = 0x03,
        SOC_H      = 0x04,
        SOC_L      = 0x05,
        RRT_ALRT_H = 0x06,
        RRT_ALRT_L = 0x07,
        CONFIG     = 0x08,
        MODE       = 0x0A,
        BATINFO    = 0x10
    };

    enum class VCELL
    {
        mask  = 0x3FFF, // battery voltage. 305uV resolution
        shift = 0
    };

    enum class SOC
    {
        mask  = 0xFFFF, // MSB - remaining charge in %. LSB Precise value - each bit is 1/256%
        shift = 0
    };

    enum class RRT_ALRT
    {
        mask  = 0x1FFF, // remaining battery time value. 1 minute per LSB accuracy
        shift = 0,
        nALRT = (1 << 15) // Alert flag. Active low
    };

    enum class CONFIG
    {
        mask  = 0xF8, // low level threshold (0% - 31%)
        shift = 3,
        UFG   = (1 << 1) // battery information update state
    };

    enum class MODE
    {
        Sleep_mask = (0x3 << 6),
        Sleep      = (0x03 << 6), // write 11b to force sleep mode
        NORMAL     = (0x00 << 6), // write 00b to force normal mode
        QSTRT      = (0x03 << 4), // write 11b to enter QSTRT mode
        POR        = 0x0F         // write 1111b to restart IC
    };

    /* got from ODM init code */
    constexpr uint8_t battery_info_config_info[BATTERY_INFO_SIZE] = {
        0x15, 0x7E, 0x80, 0x6E, 0x68, 0x64, 0x61, 0x62, 0x61, 0x5F, 0x5A, 0x56, 0x53, 0x51, 0x4F, 0x3B,
        0x2E, 0x27, 0x24, 0x24, 0x29, 0x36, 0x4C, 0x5E, 0x5E, 0x49, 0x0A, 0x3E, 0x19, 0x32, 0x44, 0x4D,
        0x4F, 0x4F, 0x52, 0x52, 0x39, 0x0E, 0x92, 0x15, 0x07, 0x15, 0x52, 0x7E, 0x8D, 0x8E, 0x8E, 0x42,
        0x60, 0x84, 0x95, 0xA5, 0x80, 0x76, 0xBD, 0xCB, 0x2F, 0x00, 0x64, 0xA5, 0xB5, 0x11, 0xB0, 0x29};

} // namespace bsp::fuel_gauge
