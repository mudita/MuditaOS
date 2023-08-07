// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051LPMCommon.hpp"
#include <log/log.hpp>
#include <fsl_clock.h>
#include <bsp/bsp.hpp>
#include "Oscillator.hpp"
#include "drivers/semc/DriverSEMC.hpp"
#include <hal/boot_reason.h>
#include <critical.hpp>

// namespace
//{
//     constexpr auto ldoStabilizationDelayUs{40};
// }

namespace bsp
{
    using namespace drivers;

    RT1051LPMCommon::RT1051LPMCommon()
    {
        driverSEMC = drivers::DriverSEMC::Create(drivers::name::ExternalRAM);
        CpuFreq    = std::make_unique<CpuFreqLPM>();
    }

    std::int32_t RT1051LPMCommon::PowerOff()
    {
        board_power_off();
        return 0;
    }

    std::int32_t RT1051LPMCommon::Reboot(RebootType reason)
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

    CpuFrequencyMHz RT1051LPMCommon::onChangeUp(CpuFrequencyMHz freq, CpuFrequencyMHz newFrequency)
    {
        if ((freq <= CpuFrequencyMHz::Level_1) && (newFrequency > CpuFrequencyMHz::Level_1)) {
            // Switch to external crystal oscillator
            SwitchOscillatorSource(LowPowerMode::OscillatorSource::External);

            // Enable regular 2P5 and 1P1 LDO, turn off weak 2P5 and 1P1 LDO
            //            SwitchToRegularModeLDO();

            // Switch external RAM clock source to PLL2
            if (driverSEMC) {
                driverSEMC->SwitchToPLL2ClockSource();
            }
        }
        return newFrequency;
    }

    void RT1051LPMCommon::onChangeDown(CpuFrequencyMHz newFrequency)
    {
        if (newFrequency <= CpuFrequencyMHz::Level_1) {
            // Switch external RAM clock source to OSC
            if (driverSEMC) {
                driverSEMC->SwitchToPeripheralClockSource();
            }

            // Enable weak 2P5 and 1P1 LDO, turn off regular 2P5 and 1P1 LDO
            //            SwitchToLowPowerModeLDO();

            // Switch to internal RC oscillator
            SwitchOscillatorSource(LowPowerMode::OscillatorSource::Internal);
        }
    }

    void RT1051LPMCommon::SetCpuFrequency(CpuFrequencyMHz freq)
    {
        if (currentFrequency == freq) {
            return;
        }

        const auto changeDirection = (currentFrequency < freq) ? Change::Up : Change::Down;
        if (Change::Up == changeDirection) {
            freq = onChangeUp(currentFrequency, freq);
        }

        switch (freq) {
        case CpuFrequencyMHz::Level_0:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_4_Mhz);
            break;
        case CpuFrequencyMHz::Level_1:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_12_Mhz);
            break;
        case CpuFrequencyMHz::Level_2:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Osc_24_Mhz);
            break;
        case CpuFrequencyMHz::Level_3:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_66_Mhz);
            break;
        case CpuFrequencyMHz::Level_4:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_132_Mhz);
            break;
        case CpuFrequencyMHz::Level_5:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_264_Mhz);
            break;
        case CpuFrequencyMHz::Level_6:
            CpuFreq->SetCpuFrequency(CpuFreqLPM::CpuClock::CpuClock_Pll2_528_Mhz);
            break;
        }

        if (Change::Down == changeDirection) {
            onChangeDown(freq);
        }

        LOG_INFO("CPU frequency changed to %lu", CLOCK_GetFreq(kCLOCK_CpuClk));
        currentFrequency = freq;
    }

    std::uint32_t RT1051LPMCommon::GetCpuFrequency() const noexcept
    {
        return CLOCK_GetCpuClkFreq();
    }

    void RT1051LPMCommon::SwitchOscillatorSource(LowPowerMode::OscillatorSource source)
    {
        switch (source) {
        case LowPowerMode::OscillatorSource::Internal:
            SwitchToInternalOscillator();
            break;

        case LowPowerMode::OscillatorSource::External:
            SwitchToExternalOscillator();
            break;
        }
    }

    void RT1051LPMCommon::RegularLDOMode()
    {
        //        cpp_freertos::CriticalSection::Enter();
        //
        //        // Enable regular 2P5 and 1P1 LDO
        //        PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_LINREG_MASK;
        //        PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_LINREG_MASK;
        //
        //        // Wait for regular LDOs to become stable (documentation Low Power AN12085)
        //        while ((PMU->REG_2P5 & PMU_REG_2P5_OK_VDD2P5_MASK) == 0) {}
        //        while ((PMU->REG_1P1 & PMU_REG_1P1_OK_VDD1P1_MASK) == 0) {}
        //
        //        // Turn off weak 2P5 and 1P1 LDO
        //        PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
        //        PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
        //
        //        cpp_freertos::CriticalSection::Exit();
    }

    void RT1051LPMCommon::LowPowerLDOMode()
    {
        //        cpp_freertos::CriticalSection::Enter();
        //
        //        // Enable weak 2P5 and 1P1 LDO
        //        PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
        //        PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
        //
        //        // Wait for weak LDOs to stabilize
        //        SDK_DelayAtLeastUs(ldoStabilizationDelayUs, CLOCK_GetCpuClkFreq());
        //
        //        // Disable regular 2P5 and 1P1 LDO
        //        PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_LINREG_MASK;
        //        PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_LINREG_MASK;
        //
        //        cpp_freertos::CriticalSection::Exit();
    }
} // namespace bsp
