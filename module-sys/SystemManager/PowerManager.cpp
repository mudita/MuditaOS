// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>

#include "PowerManager.hpp"

namespace sys
{
    PowerManager::PowerManager()
    {
        lowPowerControl = bsp::LowPowerMode::Create().value_or(nullptr);
        driverSEMC      = drivers::DriverSEMC::Create("ExternalRAM");
        cpuGovernor     = std::make_unique<CpuGovernor>();
    }

    PowerManager::~PowerManager()
    {}

    int32_t PowerManager::PowerOff()
    {
        return lowPowerControl->PowerOff();
    }

    int32_t PowerManager::Reboot()
    {
        return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::NormalRestart);
    }

    int32_t PowerManager::RebootToUpdate(UpdateReason reason)
    {
        switch (reason) {
        case UpdateReason::FactoryReset:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToUpdaterFactoryReset);
        case UpdateReason::Recovery:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToUpdaterRecovery);
        case UpdateReason::Update:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToUpdaterUpdate);
        default:
            return -1;
        }
    }

    void PowerManager::UpdateCpuFrequency(uint32_t cpuLoad)
    {
        const auto currentCpuFreq        = lowPowerControl->GetCurrentFrequencyLevel();
        const auto minFrequencyRequested = cpuGovernor->GetMinimumFrequencyRequested();

        if (cpuLoad > frequencyShiftUpperThreshold && currentCpuFreq < bsp::CpuFrequencyHz::Level_6) {
            aboveThresholdCounter++;
            belowThresholdCounter = 0;
        }
        else if (cpuLoad < frequencyShiftLowerThreshold && currentCpuFreq > bsp::CpuFrequencyHz::Level_1) {
            belowThresholdCounter++;
            aboveThresholdCounter = 0;
        }
        else {
            ResetFrequencyShiftCounter();
        }

        if (aboveThresholdCounter >= maxAboveThresholdCount || minFrequencyRequested > currentCpuFreq) {
            ResetFrequencyShiftCounter();
            IncreaseCpuFrequency();
        }
        else {
            if (belowThresholdCounter >= maxBelowThresholdCount && currentCpuFreq > minFrequencyRequested) {
                ResetFrequencyShiftCounter();
                DecreaseCpuFrequency();
            }
        }
    }

    void PowerManager::IncreaseCpuFrequency() const
    {
        const auto freq = lowPowerControl->GetCurrentFrequencyLevel();

        if (freq == bsp::CpuFrequencyHz::Level_1) {
            // Switch DCDC to full throttle during oscillator switch
            lowPowerControl->SetHighestCoreVoltage();
            // switch oscillator source
            lowPowerControl->SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource::External);
            // then switch external RAM clock source
            if (driverSEMC) {
                driverSEMC->SwitchToPLL2ClockSource();
            }
            // Add intermediate step in frequency
            SetCpuFrequency(bsp::CpuFrequencyHz::Level_4);
        }

        // and increase frequency
        if (freq < bsp::CpuFrequencyHz::Level_6) {
            SetCpuFrequency(bsp::CpuFrequencyHz::Level_6);
        }
    }

    void PowerManager::DecreaseCpuFrequency() const
    {
        const auto freq = lowPowerControl->GetCurrentFrequencyLevel();
        auto level      = bsp::CpuFrequencyHz::Level_1;

        switch (freq) {
        case bsp::CpuFrequencyHz::Level_6:
            level = bsp::CpuFrequencyHz::Level_5;
            break;
        case bsp::CpuFrequencyHz::Level_5:
            level = bsp::CpuFrequencyHz::Level_4;
            break;
        case bsp::CpuFrequencyHz::Level_4:
            level = bsp::CpuFrequencyHz::Level_3;
            break;
        case bsp::CpuFrequencyHz::Level_3:
            level = bsp::CpuFrequencyHz::Level_2;
            break;
        case bsp::CpuFrequencyHz::Level_2:
            level = bsp::CpuFrequencyHz::Level_1;
            break;
        case bsp::CpuFrequencyHz::Level_1:
            break;
        }

        // decrease frequency first
        if (level != freq) {
            SetCpuFrequency(level);
        }

        if (level == bsp::CpuFrequencyHz::Level_1) {
            // then switch osc source
            lowPowerControl->SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource::Internal);

            // and switch external RAM clock source
            if (driverSEMC) {
                driverSEMC->SwitchToPeripheralClockSource();
            }
        }
    }

    void PowerManager::RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel) const
    {
        cpuGovernor->RegisterNewSentinel(newSentinel);
    }

    void PowerManager::SetCpuFrequencyRequest(std::string sentinelName, bsp::CpuFrequencyHz request)
    {
        cpuGovernor->SetCpuFrequencyRequest(std::move(sentinelName), request);
    }

    void PowerManager::ResetCpuFrequencyRequest(std::string sentinelName)
    {
        cpuGovernor->ResetCpuFrequencyRequest(std::move(sentinelName));
    }

    void PowerManager::SetCpuFrequency(bsp::CpuFrequencyHz freq) const
    {
        lowPowerControl->SetCpuFrequency(freq);
        cpuGovernor->InformSentinelsAboutCpuFrequencyChange(freq);
    }

    void PowerManager::ResetFrequencyShiftCounter()
    {
        aboveThresholdCounter = 0;
        belowThresholdCounter = 0;
    }

    [[nodiscard]] auto PowerManager::getExternalRamDevice() const noexcept -> std::shared_ptr<devices::Device>
    {
        return driverSEMC;
    }

} // namespace sys
