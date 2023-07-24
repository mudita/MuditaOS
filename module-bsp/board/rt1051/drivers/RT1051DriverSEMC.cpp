// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverSEMC.hpp"
#include "critical.hpp"
#include "board/clock_config.h"

namespace drivers
{

    RT1051DriverSEMC::RT1051DriverSEMC(std::string name) : DriverSEMC(std::move(name))
    {
        SwitchToPLL2ClockSource();
    }

    void RT1051DriverSEMC::Enable()
    {}

    void RT1051DriverSEMC::Disable()
    {}

    void RT1051DriverSEMC::SwitchToPLL2ClockSource()
    {
        cpp_freertos::CriticalSection::Enter();
        if (!pll2Driver) {
            pll2Driver = std::make_shared<RT1051DriverPLL2>();
        }
        CLOCK_SetMux(kCLOCK_SemcMux, SemcMuxPLL2Clock);
        cpp_freertos::CriticalSection::Exit();
    }

    void RT1051DriverSEMC::SwitchToPeripheralClockSource()
    {
        cpp_freertos::CriticalSection::Enter();
        CLOCK_SetMux(kCLOCK_SemcMux, SemcMuxPeripheralClock);
        pll2Driver.reset();
        cpp_freertos::CriticalSection::Exit();
    }

} // namespace drivers
