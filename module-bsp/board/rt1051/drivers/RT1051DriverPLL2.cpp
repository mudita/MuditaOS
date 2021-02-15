// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPLL2.hpp"
#include "board/rt1051/bsp/lpm/ClockState.hpp"

namespace drivers
{

    RT1051DriverPLL2::RT1051DriverPLL2() noexcept
    {
        if (!IsPLL2Enabled()) {
            clkPLL2setup(CLK_ENABLE);
        }
    }

    RT1051DriverPLL2::~RT1051DriverPLL2()
    {
        if ((CLOCK_GetMux(kCLOCK_SemcMux) == SemcMuxPeripheralClock) && !bsp::IsClockEnabled(kCLOCK_Lpspi1) &&
            !bsp::IsClockEnabled(kCLOCK_Lpspi2) && !bsp::IsClockEnabled(kCLOCK_Lpspi3) &&
            !bsp::IsClockEnabled(kCLOCK_Lpspi4) && !bsp::IsClockEnabled(kCLOCK_Usdhc1) &&
            !bsp::IsClockEnabled(kCLOCK_Usdhc2)) {

            clkPLL2setup(CLK_DISABLE);
        }
    }

    bool RT1051DriverPLL2::IsPLL2Enabled() const noexcept
    {
        return !(CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_POWERDOWN_MASK);
    }

} // namespace drivers
