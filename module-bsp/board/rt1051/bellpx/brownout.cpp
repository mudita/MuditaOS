// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstdint>
#include <fsl_common.h>
#include <fsl_pmu.h>

namespace bsp
{
    namespace
    {
        constexpr std::uint32_t OutputVoltage1P1 = 0x19; // 1.275V
        constexpr std::uint32_t OutputVoltage2P5 = 0x19; // 2.725V

        constexpr std::uint32_t OffsetVoltage1P1 = 0x05; // 5*25mv
        constexpr std::uint32_t OffsetVoltage2P5 = 0x05; // 5*25mv
    }                                                    // namespace

    void Brownout_init()
    {
        // Config LDO Regulators
        PMU_1P1EnableOutput(PMU, true);
        PMU_1P1SetRegulatorOutputVoltage(PMU, OutputVoltage1P1);

        PMU_2P5EnableOutput(PMU, true);
        PMU_2P5SetRegulatorOutputVoltage(PMU, OutputVoltage2P5);

        // Config Brownout voltage offsets
        PMU_1P1EnableBrownout(PMU, true);
        PMU_1P1SetBrownoutOffsetVoltage(PMU, OffsetVoltage1P1);

        PMU_2P5nableBrownout(PMU, true);
        PMU_2P5SetBrownoutOffsetVoltage(PMU, OffsetVoltage2P5);
    }

} // namespace bsp
