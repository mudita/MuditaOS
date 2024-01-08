// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <fsl_common.h>
#include <fsl_pmu.h>

namespace bsp
{
    namespace
    {
        constexpr auto outputVoltage2P5 = 0x1BU; // 2.775V
        constexpr auto offsetVoltage2P5 = 0x03U; // 3*25mV
    }                                            // namespace

    void Brownout_init()
    {
        PMU_2P5nableBrownout(PMU, true);
        PMU_2P5SetRegulatorOutputVoltage(PMU, outputVoltage2P5);
        PMU_2P5SetBrownoutOffsetVoltage(PMU, offsetVoltage2P5);
        PMU_2P5EnableOutput(PMU, true);
    }
} // namespace bsp
