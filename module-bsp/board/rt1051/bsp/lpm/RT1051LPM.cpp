// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051LPM.hpp"

#include "board.h"
#include "reboot_codes.hpp"
#include <log.hpp>
#include "board/BoardDefinitions.hpp"
#include "bsp/watchdog/watchdog.hpp"
#include <board/clock_config.h>
#include <fsl_clock.h>
#include "ClockState.hpp"
#include "Oscillator.hpp"
#include "critical.hpp"

namespace bsp
{

    using namespace drivers;

    RT1051LPM::RT1051LPM()
    {
        gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::POWER_SWITCH_HOLD_GPIO),
                                    DriverGPIOParams{});
        gpio_2 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::DCDC_INVERTER_MODE_GPIO),
                                    DriverGPIOParams{});

        gpio_1->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 1,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::DCDC_INVERTER_MODE_PIN)});

        gpio_1->WritePin(static_cast<uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON), 1);
        DisableDcdcPowerSaveMode();

        CpuFreq = std::make_unique<CpuFreqLPM>();
    }

    int32_t RT1051LPM::PowerOff()
    {
        gpio_1->WritePin(static_cast<uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON), 0);
        return 0;
    }

    int32_t RT1051LPM::Reboot(RebootType reason)
    {
        switch (reason) {
        case RebootType::GoToUpdaterUpdate:
            SNVS->LPGPR[0] = bsp::rebootCode::rebootToUpdateCode;
            break;
        case RebootType::GoToUpdaterRecovery:
            SNVS->LPGPR[0] = bsp::rebootCode::rebootToRecoveryCode;
            break;
        case RebootType::GoToUpdaterFactoryReset:
            SNVS->LPGPR[0] = bsp::rebootCode::rebootToFactoryRstCode;
            break;
        case RebootType::NormalRestart:
            SNVS->LPGPR[0] = bsp::rebootCode::rebootNormalCode;
            break;
        }
        NVIC_SystemReset();
        return 0;
    }

    void RT1051LPM::SetCpuFrequency(bsp::CpuFrequencyHz freq)
    {
        currentFrequency = freq;
        switch (freq) {
        case bsp::CpuFrequencyHz::Level_1:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_12_Mhz);
            break;
        case bsp::CpuFrequencyHz::Level_2:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_24_Mhz);
            break;
        case bsp::CpuFrequencyHz::Level_3:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_66_Mhz);
            break;
        case bsp::CpuFrequencyHz::Level_4:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_132_Mhz);
            break;
        case bsp::CpuFrequencyHz::Level_5:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_264_Mhz);
            break;
        case bsp::CpuFrequencyHz::Level_6:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_528_Mhz);
            break;
        }
        LOG_INFO("CPU frequency changed to %lu", CLOCK_GetFreq(kCLOCK_CpuClk));
    }

    void RT1051LPM::SetHighestCoreVoltage()
    {
        CpuFreq->SetHighestCoreVoltage();
    }

    uint32_t RT1051LPM::GetCpuFrequency() const noexcept
    {
        return CLOCK_GetCpuClkFreq();
    }

    void RT1051LPM::SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource source)
    {
        if (source == bsp::LowPowerMode::OscillatorSource::Internal) {
            cpp_freertos::CriticalSection::Enter();
            bsp::DisableExternalOscillator();
            cpp_freertos::CriticalSection::Exit();
        }
        else if (source == bsp::LowPowerMode::OscillatorSource::External) {
            cpp_freertos::CriticalSection::Enter();
            bsp::EnableExternalOscillator();
            cpp_freertos::CriticalSection::Exit();
        }
    }

    void RT1051LPM::SetBootSuccess()
    {
        SNVS->LPGPR[0] = bsp::rebootCode::rebootNormalCode;
    }

    void RT1051LPM::EnableDcdcPowerSaveMode()
    {
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::DCDC_INVERTER_MODE_PIN), 0);
    }

    void RT1051LPM::DisableDcdcPowerSaveMode()
    {
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::DCDC_INVERTER_MODE_PIN), 1);
    }

} // namespace bsp
