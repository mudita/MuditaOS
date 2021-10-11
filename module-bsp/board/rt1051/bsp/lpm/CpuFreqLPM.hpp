// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_CPUFREQLPM_HPP
#define PUREPHONE_CPUFREQLPM_HPP

#include <cstdint>

namespace bsp
{
    inline constexpr uint32_t VDDRun_900_mV  = 0x4;
    inline constexpr uint32_t VDDRun_1050_mV = 0xa;
    inline constexpr uint32_t VDDRun_1075_mV = 0xb;
    inline constexpr uint32_t VDDRun_1100_mV = 0xc;
    inline constexpr uint32_t VDDRun_1125_mV = 0xd;
    inline constexpr uint32_t VDDRun_1150_mV = 0xe;
    inline constexpr uint32_t VDDRun_1275_mV = 0x13;

    inline constexpr uint32_t VDDStandby_925_mV = 0x1;

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

#endif // PUREPHONE_CPUFREQLPM_HPP
