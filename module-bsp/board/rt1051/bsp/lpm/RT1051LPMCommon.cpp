// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051LPMCommon.hpp"

#include <log/log.hpp>
#include <fsl_clock.h>
#include <bsp/bsp.hpp>
#include "Oscillator.hpp"
#include "critical.hpp"
#include "drivers/semc/DriverSEMC.hpp"
#include <hal/boot_reason.h>

namespace bsp
{

    using namespace drivers;

    RT1051LPMCommon::RT1051LPMCommon()
    {
        driverSEMC = drivers::DriverSEMC::Create(drivers::name::ExternalRAM);
        CpuFreq    = std::make_unique<CpuFreqLPM>();
    }

    int32_t RT1051LPMCommon::PowerOff()
    {
        board_power_off();
        return 0;
    }

    int32_t RT1051LPMCommon::Reboot(RebootType reason)
    {
        switch (reason) {
        case RebootType::GoToRecoveryUpdate:
            set_boot_reason(boot_reason_code_update);
            break;
        case RebootType::GoToRecoveryRecovery:
            set_boot_reason(boot_reason_code_recovery);
            break;
        case RebootType::GoToRecoveryFactoryReset:
            set_boot_reason(boot_reason_code_factory);
            break;
        case RebootType::GoToRecoveryBackup:
            set_boot_reason(boot_reason_code_backup);
            break;
        case RebootType::GoToRecoveryRestore:
            set_boot_reason(boot_reason_code_restore);
            break;
        case RebootType::NormalRestart:
            set_boot_reason(boot_reason_code_os);
            break;
        default:
            set_boot_reason(boot_reason_code_unknown);
        }
        board_restart();
        return 0;
    }

    enum class Change
    {
        Up,
        Down
    };

    CpuFrequencyMHz RT1051LPMCommon::onChangeUp(CpuFrequencyMHz freq, bsp::CpuFrequencyMHz newFrequency)
    {
        if ((freq <= CpuFrequencyMHz::Level_1) && (newFrequency > CpuFrequencyMHz::Level_1)) {
            // connect internal the load resistor
            ConnectInternalLoadResistor();
            // Switch DCDC to full throttle during oscillator switch
            SetHighestCoreVoltage();
            // Enable regular 2P5 and 1P1 LDO and Turn off weak 2P5 and 1P1 LDO
            SwitchToRegularModeLDO();
            // switch oscillator source
            SwitchOscillatorSource(LowPowerMode::OscillatorSource::External);
            // then switch external RAM clock source
            if (driverSEMC) {
                driverSEMC->SwitchToPLL2ClockSource();
            }
            // Add intermediate step in frequency
            if (newFrequency > CpuFrequencyMHz::Level_4) {
                return CpuFrequencyMHz::Level_4;
            }
        }
        return newFrequency;
    }

    void RT1051LPMCommon::onChangeDown(CpuFrequencyMHz newFrequency)
    {
        if (newFrequency <= bsp::CpuFrequencyMHz::Level_1) {
            // Enable weak 2P5 and 1P1 LDO and Turn off regular 2P5 and 1P1 LDO
            SwitchToLowPowerModeLDO();
            // then switch osc source
            SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource::Internal);
            // and switch external RAM clock source
            if (driverSEMC) {
                driverSEMC->SwitchToPeripheralClockSource();
            }

            // disconnect internal the load resistor
            DisconnectInternalLoadResistor();
        }
    }

    void RT1051LPMCommon::SetCpuFrequency(bsp::CpuFrequencyMHz freq)
    {
        if (currentFrequency == freq) {
            return;
        }
        Change change = currentFrequency < freq ? Change::Up : Change::Down;
        if (Change::Up == change) {
            freq = onChangeUp(currentFrequency, freq);
        }
        switch (freq) {
        case bsp::CpuFrequencyMHz::Level_0:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_4_Mhz);
            break;
        case bsp::CpuFrequencyMHz::Level_1:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_12_Mhz);
            break;
        case bsp::CpuFrequencyMHz::Level_2:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_24_Mhz);
            break;
        case bsp::CpuFrequencyMHz::Level_3:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_66_Mhz);
            break;
        case bsp::CpuFrequencyMHz::Level_4:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_132_Mhz);
            break;
        case bsp::CpuFrequencyMHz::Level_5:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_264_Mhz);
            break;
        case bsp::CpuFrequencyMHz::Level_6:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_528_Mhz);
            break;
        }

        if (Change::Down == change) {
            onChangeDown(freq);
        }
        LOG_INFO("CPU frequency changed to %lu", CLOCK_GetFreq(kCLOCK_CpuClk));
        currentFrequency = freq;
    }

    void RT1051LPMCommon::SetHighestCoreVoltage()
    {
        CpuFreq->SetHighestCoreVoltage();
    }

    uint32_t RT1051LPMCommon::GetCpuFrequency() const noexcept
    {
        return CLOCK_GetCpuClkFreq();
    }

    void RT1051LPMCommon::SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource source)
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

    void RT1051LPMCommon::DisconnectInternalLoadResistor()
    {
        DCDC->REG1 &= ~DCDC_REG1_REG_RLOAD_SW_MASK;
    }

    void RT1051LPMCommon::ConnectInternalLoadResistor()
    {
        DCDC->REG1 |= DCDC_REG1_REG_RLOAD_SW_MASK;
    }

    void RT1051LPMCommon::RegularLDOMode()
    {
        // Enable regular 2P5 and wait it stable
        PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_LINREG_MASK;
        // It is recommended to wait for stabilization (documentation Low Power AN12085)
        while ((PMU->REG_2P5 & PMU_REG_2P5_OK_VDD2P5_MASK) == 0) {}
        // Turn off weak 2P5
        PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
        // Enable regular 1P1 and wait for stable
        PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_LINREG_MASK;
        // It is recommended to wait for stabilization (documentation Low Power AN12085)
        while ((PMU->REG_1P1 & PMU_REG_1P1_OK_VDD1P1_MASK) == 0) {}
        // Turn off weak 1P1
        PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
    }

    void RT1051LPMCommon::LowPowerLDOMode()
    {
        // Enable weak 2P5 and turn off regular 2P5
        PMU->REG_2P5 |= PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
        PMU->REG_2P5 &= ~PMU_REG_2P5_ENABLE_LINREG_MASK;
        // Enable weak 1P1 and turn off regular 1P1
        PMU->REG_1P1 |= PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
        PMU->REG_1P1 &= ~PMU_REG_1P1_ENABLE_LINREG_MASK;
    }
} // namespace bsp
