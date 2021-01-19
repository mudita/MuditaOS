// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverUSDHC.hpp"
#include "critical.hpp"
#include "board/rt1051/bsp/lpm/ClockState.hpp"

namespace drivers
{

    RT1051DriverUSDHC::RT1051DriverUSDHC(std::string name, USDHCInstances inst) : DriverUSDHC(std::move(name), inst)
    {
        switch (instance) {
        case USDHCInstances::USDHC_1:
            usdhcClock = kCLOCK_Usdhc1;
            break;
        case USDHCInstances::USDHC_2:
            usdhcClock = kCLOCK_Usdhc2;
            break;
        }
    }

    void RT1051DriverUSDHC::Enable()
    {
        cpp_freertos::CriticalSection::Enter();
        if (!pll2Driver) {
            pll2Driver = std::make_shared<RT1051DriverPLL2>();
        }
        if (!bsp::IsClockEnabled(usdhcClock)) {
            CLOCK_EnableClock(usdhcClock);
        }
        cpp_freertos::CriticalSection::Exit();
    }

    void RT1051DriverUSDHC::Disable()
    {
        cpp_freertos::CriticalSection::Enter();
        if (bsp::IsClockEnabled(usdhcClock)) {
            CLOCK_DisableClock(usdhcClock);
        }
        pll2Driver.reset();
        cpp_freertos::CriticalSection::Exit();
    }

} // namespace drivers
