// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051LPM.hpp"

#include "board.h"
#include "log/log.hpp"
#include "bsp/BoardDefinitions.hpp"
#include "bsp/watchdog/watchdog.hpp"

namespace bsp
{

    using namespace drivers;

    RT1051LPM::RT1051LPM()
    {
        gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::POWER_SWITCH_HOLD_GPIO),
                                  DriverGPIOParams{});

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                          .defLogic = 1,
                                          .pin = static_cast<uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON)});

        gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON), 1);

        CpuFreq = std::make_unique<CpuFreqLPM>();
    }

    int32_t RT1051LPM::Switch(const bsp::LowPowerMode::Mode mode)
    {
        currentMode = mode;
        switch (mode) {
        case Mode ::FullSpeed:
            return EnterFullSpeed();
        case Mode ::LowPowerRun:
            return EnterLowPowerRun();
        case Mode ::LowPowerIdle:
            return EnterLowPowerIdle();
        case Mode ::Suspend:
            return EnterSuspend();
        default:
            return 0;
        }
    }

    int32_t RT1051LPM::PowerOff()
    {
        gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON), 0);
        return 0;
    }

    int32_t RT1051LPM::Reboot()
    {
        bsp::watchdog::init();
        bsp::watchdog::system_reset();
        return 0;
    }

    int32_t RT1051LPM::EnterFullSpeed()
    {
        LPM_EnterFullSpeed();
        return 0;
    }

    int32_t RT1051LPM::EnterLowPowerRun()
    {
        LPM_EnterLowPowerRun();
        return 0;
    }

    int32_t RT1051LPM::EnterLowPowerIdle()
    {
        LPM_EnterLowPowerIdle();
        return 0;
    }

    int32_t RT1051LPM::EnterSuspend()
    {
        return 0;
    }

    void RT1051LPM::SetCpuFrequency(bsp::LowPowerMode::CpuFrequency freq)
    {
        switch (freq) {
        case bsp::LowPowerMode::CpuFrequency::Level_1:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_12_Mhz);
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_2:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_24_Mhz);
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_3:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_66_Mhz);
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_4:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_132_Mhz);
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_5:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_264_Mhz);
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_6:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_528_Mhz);
            break;
        }
        LOG_INFO("CPU frequency changed to %lu", CLOCK_GetFreq(kCLOCK_CpuClk));
    }
} // namespace bsp
