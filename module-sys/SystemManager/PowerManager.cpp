// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>

#include "PowerManager.hpp"

namespace sys
{
    namespace
    {
        inline constexpr uint32_t frequencyShiftLowerThreshold{40};
        inline constexpr uint32_t frequencyShiftUpperThreshold{60};
        inline constexpr uint32_t maxBelowThresholdCount{30};
        inline constexpr uint32_t maxBelowThresholdInRowCount{10};
        inline constexpr uint32_t maxAboveThresholdCount{3};
        constexpr auto lowestLevelName{"lowestCpuFrequency"};
        constexpr auto middleLevelName{"middleCpuFrequency"};
        constexpr auto highestLevelName{"highestCpuFrequency"};
    } // namespace

    CpuFrequencyMonitor::CpuFrequencyMonitor(const std::string name) : levelName(name)
    {}

    [[nodiscard]] auto CpuFrequencyMonitor::GetName() const noexcept -> std::string
    {
        return levelName;
    }

    [[nodiscard]] auto CpuFrequencyMonitor::GetRuntimePercentage() const noexcept -> std::uint32_t
    {
        auto tickCount = xTaskGetTickCount();
        return tickCount == 0 ? 0 : ((totalTicksCount * 100) / tickCount);
    }

    void CpuFrequencyMonitor::IncreaseTicks(TickType_t ticks)
    {
        totalTicksCount += ticks;
    }

    PowerManager::PowerManager()
    {
        lowPowerControl = bsp::LowPowerMode::Create().value_or(nullptr);
        driverSEMC      = drivers::DriverSEMC::Create("ExternalRAM");
        cpuGovernor     = std::make_unique<CpuGovernor>();

        cpuFrequencyMonitor.push_back(CpuFrequencyMonitor(lowestLevelName));
        cpuFrequencyMonitor.push_back(CpuFrequencyMonitor(middleLevelName));
        cpuFrequencyMonitor.push_back(CpuFrequencyMonitor(highestLevelName));
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

        if (!belowThresholdCounter) {
            isFrequencyLoweringInProgress = false;
        }

        if (aboveThresholdCounter >= maxAboveThresholdCount || minFrequencyRequested > currentCpuFreq) {
            ResetFrequencyShiftCounter();
            IncreaseCpuFrequency();
        }
        else {
            if (belowThresholdCounter >=
                    (isFrequencyLoweringInProgress ? maxBelowThresholdInRowCount : maxBelowThresholdCount) &&
                currentCpuFreq > minFrequencyRequested) {
                ResetFrequencyShiftCounter();
                DecreaseCpuFrequency();
            }
        }
    }

    void PowerManager::IncreaseCpuFrequency()
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

    void PowerManager::DecreaseCpuFrequency()
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

        isFrequencyLoweringInProgress = true;
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

    void PowerManager::SetCpuFrequency(bsp::CpuFrequencyHz freq)
    {
        const auto currentFreq = lowPowerControl->GetCurrentFrequencyLevel();
        UpdateCpuFrequencyMonitor(
            currentFreq == bsp::CpuFrequencyHz::Level_1
                ? lowestLevelName
                : (currentFreq == bsp::CpuFrequencyHz::Level_6 ? highestLevelName : middleLevelName));
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

    void PowerManager::UpdateCpuFrequencyMonitor(const std::string levelName)
    {
        auto ticks = xTaskGetTickCount();

        for (auto &level : cpuFrequencyMonitor) {
            if (level.GetName() == levelName) {
                level.IncreaseTicks(ticks - lastCpuFrequencyChangeTimestamp);
            }
        }

        lastCpuFrequencyChangeTimestamp = ticks;
    }

    void PowerManager::LogPowerManagerEfficiency()
    {
        std::string log{"PowerManager Efficiency: "};

        for (auto &level : cpuFrequencyMonitor) {
            log.append(level.GetName() + ": " + std::to_string(level.GetRuntimePercentage()) + "% ");
        }

        LOG_INFO("%s", log.c_str());
    }

} // namespace sys
