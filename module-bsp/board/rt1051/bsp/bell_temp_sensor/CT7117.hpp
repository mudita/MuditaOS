// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace bsp::bell_temp_sensor
{
    constexpr inline auto CT7117_DEVICE_ADDR = 0x90;
    constexpr inline auto CT7117_DEVICE_ID = 0x59;

    constexpr inline auto CT7117_Temperature_LSB = 0.0078125;

    enum class CT7117_Registers
    {
        Temp = 0x00,
        Config = 0x01,
        Low_Temp_Set = 0x02,
        High_Temp_Set = 0x03,
        ID = 0x07
    };

    enum class CT7117_Config_Reg
    {
        OTS = (1<<15),
        F1 = (1 << 12),
        F0 = (1 << 11),
        ALTM = (1 << 9),
        SD = (1 << 8),
        EM = (1 << 7),
        RES1 = (1 << 6),
        RES0 = (1 << 5),
        TO = (1 << 4),
        PEC = (1 << 3),
        CR1 = (1 << 2),
        CR0 = (1 << 1),
        OS = (1 << 0)
    }; 

} // namespace bsp::bell_temp_sensor
