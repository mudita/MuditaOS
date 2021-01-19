// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>

#include "PowerManager.hpp"

namespace sys
{
    PowerManager::PowerManager()
    {
        lowPowerControl = bsp::LowPowerMode::Create().value_or(nullptr);
        driverSEMC      = drivers::DriverSEMC::Create("ExternalRAM");
    }

    PowerManager::~PowerManager()
    {}

    int32_t PowerManager::PowerOff()
    {
        return lowPowerControl->PowerOff();
    }

    int32_t PowerManager::Reboot()
    {
        return lowPowerControl->Reboot();
    }

    void PowerManager::UpdateCpuFrequency(uint32_t cpuLoad)
    {
        const auto freq = lowPowerControl->GetCurrentFrequency();

        if (cpuLoad > frequencyShiftUpperThreshold && freq < bsp::LowPowerMode::CpuFrequency::Level_6) {
            aboveThresholdCounter++;
            belowThresholdCounter = 0;
        }
        else if (cpuLoad < frequencyShiftLowerThreshold && freq > bsp::LowPowerMode::CpuFrequency::Level_1) {
            belowThresholdCounter++;
            aboveThresholdCounter = 0;
        }
        else {
            ResetFrequencyShiftCounter();
        }

        if (aboveThresholdCounter >= maxAboveThresholdCount) {
            ResetFrequencyShiftCounter();
            IncreaseCpuFrequency();
        }
        if (belowThresholdCounter >= maxBelowThresholdCount) {
            ResetFrequencyShiftCounter();
            DecreaseCpuFrequency();
        }
    }

    void PowerManager::IncreaseCpuFrequency() const
    {
        const auto freq      = lowPowerControl->GetCurrentFrequency();
        const auto oscSource = lowPowerControl->GetCurrentOscillatorSource();

        if (freq == bsp::LowPowerMode::CpuFrequency::Level_1) {
            // switch osc source first
            if (oscSource == bsp::LowPowerMode::OscillatorSource::Internal) {
                lowPowerControl->SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource::External);
            }

            // then switch external RAM clock source
            if (driverSEMC) {
                driverSEMC->SwitchToPLL2ClockSource();
            }
        }

        // and increase frequency
        if (freq < bsp::LowPowerMode::CpuFrequency::Level_6) {
            lowPowerControl->SetCpuFrequency(bsp::LowPowerMode::CpuFrequency::Level_6);
        }
    }

    void PowerManager::DecreaseCpuFrequency() const
    {
        const auto freq = lowPowerControl->GetCurrentFrequency();
        auto level      = bsp::LowPowerMode::CpuFrequency::Level_1;

        // We temporarily limit the minimum CPU frequency
        // due to problems with the UART of the GSM modem
        switch (freq) {
        case bsp::LowPowerMode::CpuFrequency::Level_6:
            level = bsp::LowPowerMode::CpuFrequency::Level_5;
            break;
        default:
            level = bsp::LowPowerMode::CpuFrequency::Level_4;
            break;
        }

        // decrease frequency first
        if (level != freq) {
            lowPowerControl->SetCpuFrequency(level);
        }

        if (level == bsp::LowPowerMode::CpuFrequency::Level_1) {
            const auto oscSource = lowPowerControl->GetCurrentOscillatorSource();

            // then switch osc source
            if (oscSource == bsp::LowPowerMode::OscillatorSource::External) {
                lowPowerControl->SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource::Internal);
            }

            // and switch external RAM clock source
            if (driverSEMC) {
                driverSEMC->SwitchToPeripheralClockSource();
            }
        }
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
