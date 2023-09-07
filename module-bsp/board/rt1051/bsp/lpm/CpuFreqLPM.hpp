// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace bsp
{
    inline constexpr std::uint32_t VDDRun_950_mV  = 0x06;
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

        /* Plain enums to avoid casting when passing to FSL functions */
        enum AhbDivValues
        {
            AhbDiv1 = 0,
            AhbDiv2,
            AhbDiv3,
            AhbDiv4,
            AhbDiv5,
            AhbDiv6,
            AhbDiv7,
            AhbDiv8
        };

        enum PeriphMuxSources
        {
            PeriphMuxSourcePll2_528MHz = 0,
            PeriphMuxSourceOsc_24MHz
        };

        CpuFreqLPM();
        void SetCpuFrequency(CpuClock freq);
    };
} // namespace bsp
