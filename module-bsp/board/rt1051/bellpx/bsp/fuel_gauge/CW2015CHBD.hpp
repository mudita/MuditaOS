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
        // profile_DEM50X_2nd_20211012
        0x15, 0x15, 0x6E, 0x67, 0x65, 0x62, 0x60, 0x60, 0x5F, 0x5E, 0x5B, 0x59, 0x55, 0x50, 0x41, 0x33,
        0x2A, 0x26, 0x24, 0x27, 0x31, 0x46, 0x55, 0x5B, 0x47, 0x4A, 0x0A, 0x3E, 0x38, 0x58, 0x59, 0x63,
        0x67, 0x63, 0x62, 0x64, 0x3D, 0x1B, 0x6F, 0x15, 0x07, 0x21, 0x54, 0x85, 0x8F, 0x90, 0x90, 0x44,
        0x63, 0x86, 0x94, 0x99, 0x80, 0x89, 0xBC, 0xCB, 0x2F, 0x00, 0x64, 0xA5, 0xB5, 0xC1, 0x46, 0xAE};

} // namespace bsp::fuel_gauge
