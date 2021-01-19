// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051LPM.hpp"

#include "board.h"
#include "log/log.hpp"
#include "bsp/BoardDefinitions.hpp"
#include "bsp/watchdog/watchdog.hpp"
#include <clock_config.h>
#include <fsl_clock.h>
#include <fsl_dcdc.h>
#include "ClockState.hpp"

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

    void RT1051LPM::SetCpuFrequency(bsp::LowPowerMode::CpuFrequency freq)
    {
        currentFrequency = freq;
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

    void RT1051LPM::SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource source)
    {
        if (source == bsp::LowPowerMode::OscillatorSource::Internal) {
            if (IsClockEnabled(kCLOCK_Lpuart1) || IsClockEnabled(kCLOCK_Lpuart2) || IsClockEnabled(kCLOCK_Lpuart3) ||
                IsClockEnabled(kCLOCK_Lpuart4) || IsClockEnabled(kCLOCK_Lpuart5) || IsClockEnabled(kCLOCK_Lpuart6) ||
                IsClockEnabled(kCLOCK_Lpuart7) || IsClockEnabled(kCLOCK_Lpuart8)) {
                return;
            }

            /// Switch DCDC to use DCDC internal OSC
            DCDC_SetClockSource(DCDC, kDCDC_ClockInternalOsc);
            /// Switch clock source to internal RC
            CLOCK_SwitchOsc(kCLOCK_RcOsc);
            CLOCK_DeinitExternalClk();
            /// Wait CCM operation finishes
            while (CCM->CDHIPR != 0) {}
        }
        else if (source == bsp::LowPowerMode::OscillatorSource::External) {
            CLOCK_InitExternalClk(0);
            /// Switch DCDC to use DCDC external OSC
            DCDC_SetClockSource(DCDC, kDCDC_ClockExternalOsc);
            /// Switch clock source to external OSC.
            CLOCK_SwitchOsc(kCLOCK_XtalOsc);
            /// Wait CCM operation finishes
            while (CCM->CDHIPR != 0) {}
            /// Set Oscillator ready counter value.
            CCM->CCR = (CCM->CCR & (~CCM_CCR_OSCNT_MASK)) | CCM_CCR_OSCNT(bsp::OscillatorReadyCounterValue);
        }

        currentOscSource = source;
    }

} // namespace bsp
