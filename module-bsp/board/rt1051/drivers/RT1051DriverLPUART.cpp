// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverLPUART.hpp"
#include "critical.hpp"
#include "board/rt1051/bsp/lpm/ClockState.hpp"

namespace drivers
{

    RT1051DriverLPUART::RT1051DriverLPUART(std::string name, LPUARTInstances inst) : DriverLPUART(std::move(name), inst)
    {
        switch (instance) {
        case LPUARTInstances::LPUART_1:
            lpuartClock = kCLOCK_Lpuart1;
            break;
        case LPUARTInstances::LPUART_2:
            lpuartClock = kCLOCK_Lpuart2;
            break;
        case LPUARTInstances::LPUART_3:
            lpuartClock = kCLOCK_Lpuart3;
            break;
        case LPUARTInstances::LPUART_4:
            lpuartClock = kCLOCK_Lpuart4;
            break;
        case LPUARTInstances::LPUART_5:
            lpuartClock = kCLOCK_Lpuart5;
            break;
        case LPUARTInstances::LPUART_6:
            lpuartClock = kCLOCK_Lpuart6;
            break;
        case LPUARTInstances::LPUART_7:
            lpuartClock = kCLOCK_Lpuart7;
            break;
        case LPUARTInstances::LPUART_8:
            lpuartClock = kCLOCK_Lpuart8;
            break;
        }
    }

    void RT1051DriverLPUART::Enable()
    {
        cpp_freertos::CriticalSection::Enter();
        if (!oscillatorDriver) {
            oscillatorDriver = std::make_shared<RT1051DriverOscillator>();
        }
        if (!bsp::IsClockEnabled(lpuartClock)) {
            CLOCK_EnableClock(lpuartClock);
        }
        cpp_freertos::CriticalSection::Exit();
    }

    void RT1051DriverLPUART::Disable()
    {
        cpp_freertos::CriticalSection::Enter();
        if (bsp::IsClockEnabled(lpuartClock)) {
            CLOCK_DisableClock(lpuartClock);
        }
        oscillatorDriver.reset();
        cpp_freertos::CriticalSection::Exit();
    }

} // namespace drivers
