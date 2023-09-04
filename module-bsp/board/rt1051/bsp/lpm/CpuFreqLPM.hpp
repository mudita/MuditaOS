// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace bsp
{
    inline constexpr std::uint32_t VDDRun_975_mV  = 0x07;
    inline constexpr std::uint32_t VDDRun_1050_mV = 0x0A;
    inline constexpr std::uint32_t VDDRun_1075_mV = 0x0B;
    inline constexpr std::uint32_t VDDRun_1100_mV = 0x0C;
    inline constexpr std::uint32_t VDDRun_1125_mV = 0x0D;
    inline constexpr std::uint32_t VDDRun_1150_mV = 0x0E;
    inline constexpr std::uint32_t VDDRun_1275_mV = 0x13;

    inline constexpr std::uint32_t VDDStandby_925_mV = 0x01;

    class CpuFreqLPM
    {
      public:
        enum class CpuClock
        {
            CpuClock_Osc_4_Mhz,
            CpuClock_Osc_12_Mhz,
            CpuClock_Osc_24_Mhz,
            CpuClock_Pll2_66_Mhz,
            CpuClock_Pll2_132_Mhz,
            CpuClock_Pll2_264_Mhz,
            CpuClock_Pll2_528_Mhz
        };

        CpuFreqLPM();
        void SetCpuFrequency(CpuClock freq);
        void SetHighestCoreVoltage();
    };
} // namespace bsp
