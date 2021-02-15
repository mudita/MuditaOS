// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverLPSPI.hpp"
#include "critical.hpp"
#include "board/rt1051/bsp/lpm/ClockState.hpp"

namespace drivers
{

    RT1051DriverLPSPI::RT1051DriverLPSPI(std::string name, LPSPIInstances inst) : DriverLPSPI(std::move(name), inst)
    {
        switch (instance) {
        case LPSPIInstances::LPSPI_1:
            lpspiClock = kCLOCK_Lpspi1;
            break;
        case LPSPIInstances::LPSPI_2:
            lpspiClock = kCLOCK_Lpspi2;
            break;
        case LPSPIInstances::LPSPI_3:
            lpspiClock = kCLOCK_Lpspi3;
            break;
        case LPSPIInstances::LPSPI_4:
            lpspiClock = kCLOCK_Lpspi4;
            break;
        }
    }

    void RT1051DriverLPSPI::Enable()
    {
        cpp_freertos::CriticalSection::Enter();
        if (!pll2Driver) {
            pll2Driver = std::make_shared<RT1051DriverPLL2>();
        }
        if (!bsp::IsClockEnabled(lpspiClock)) {
            CLOCK_EnableClock(lpspiClock);
        }
        cpp_freertos::CriticalSection::Exit();
    }

    void RT1051DriverLPSPI::Disable()
    {
        cpp_freertos::CriticalSection::Enter();
        if (bsp::IsClockEnabled(lpspiClock)) {
            CLOCK_DisableClock(lpspiClock);
        }
        pll2Driver.reset();
        cpp_freertos::CriticalSection::Exit();
    }

} // namespace drivers
