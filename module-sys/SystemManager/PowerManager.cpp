// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>

#include "PowerManager.hpp"

namespace sys
{
    PowerManager::PowerManager()
    {
        lowPowerControl = bsp::LowPowerMode::Create().value_or(nullptr);
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

        /// switch osc source first
        if (freq == bsp::LowPowerMode::CpuFrequency::Level_1 &&
            oscSource == bsp::LowPowerMode::OscillatorSource::Internal) {
            lowPowerControl->SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource::External);
        }

        /// then increase frequency
        if (freq < bsp::LowPowerMode::CpuFrequency::Level_6) {
            lowPowerControl->SetCpuFrequency(bsp::LowPowerMode::CpuFrequency::Level_6);
        }
    }

    void PowerManager::DecreaseCpuFrequency() const
    {
        const auto freq = lowPowerControl->GetCurrentFrequency();
        const auto oscSource = lowPowerControl->GetCurrentOscillatorSource();
        auto level      = bsp::LowPowerMode::CpuFrequency::Level_1;

        switch (freq) {
        case bsp::LowPowerMode::CpuFrequency::Level_6:
            level = bsp::LowPowerMode::CpuFrequency::Level_5;
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_5:
            level = bsp::LowPowerMode::CpuFrequency::Level_4;
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_4:
            level = bsp::LowPowerMode::CpuFrequency::Level_3;
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_3:
            level = bsp::LowPowerMode::CpuFrequency::Level_2;
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_2:
            level = bsp::LowPowerMode::CpuFrequency::Level_1;
            break;
        case bsp::LowPowerMode::CpuFrequency::Level_1:
            break;
        }

        /// decrease frequency first
        if (level != freq) {
            lowPowerControl->SetCpuFrequency(level);
        }

        /// then switch osc source
        if (level == bsp::LowPowerMode::CpuFrequency::Level_1 &&
            oscSource == bsp::LowPowerMode::OscillatorSource::External) {
            lowPowerControl->SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource::Internal);
        }
    }

    void PowerManager::ResetFrequencyShiftCounter()
    {
        aboveThresholdCounter = 0;
        belowThresholdCounter = 0;
    }
} // namespace sys
