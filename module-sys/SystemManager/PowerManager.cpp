// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>

#include <SystemManager/PowerManager.hpp>

namespace sys
{
    namespace
    {
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

    PowerManager::PowerManager() : powerProfile{bsp::getPowerProfile()}
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

    int32_t PowerManager::RebootToUsbMscMode()
    {
        return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToUsbMscMode);
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
        const auto currentCpuFreq           = lowPowerControl->GetCurrentFrequencyLevel();
        const auto minFrequencyRequested    = cpuGovernor->GetMinimumFrequencyRequested();
        const auto permanentFrequencyToHold = cpuGovernor->GetPermanentFrequencyRequested();

        if (permanentFrequencyToHold.isActive) {
            auto frequencyToHold = std::max(permanentFrequencyToHold.frequencyToHold, powerProfile.minimalFrequency);

            if (currentCpuFreq < frequencyToHold) {
                IncreaseCpuFrequency(frequencyToHold);
            }
            else if (currentCpuFreq > frequencyToHold) {
                do {
                    DecreaseCpuFrequency();
                } while (lowPowerControl->GetCurrentFrequencyLevel() > frequencyToHold);
            }
            ResetFrequencyShiftCounter();
            return;
        }

        if (cpuLoad > powerProfile.frequencyShiftUpperThreshold && currentCpuFreq < bsp::CpuFrequencyMHz::Level_6) {
            aboveThresholdCounter++;
            belowThresholdCounter = 0;
        }
        else if (cpuLoad < powerProfile.frequencyShiftLowerThreshold &&
                 currentCpuFreq > powerProfile.minimalFrequency) {
            belowThresholdCounter++;
            aboveThresholdCounter = 0;
        }
        else {
            ResetFrequencyShiftCounter();
        }

        if (!belowThresholdCounter) {
            isFrequencyLoweringInProgress = false;
        }

        if (minFrequencyRequested > currentCpuFreq) {
            ResetFrequencyShiftCounter();
            IncreaseCpuFrequency(minFrequencyRequested);
        }
        else if (aboveThresholdCounter >= powerProfile.maxAboveThresholdCount) {
            if (powerProfile.frequencyIncreaseIntermediateStep && currentCpuFreq < bsp::CpuFrequencyMHz::Level_4) {
                ResetFrequencyShiftCounter();
                IncreaseCpuFrequency(bsp::CpuFrequencyMHz::Level_4);
            }
            else {
                ResetFrequencyShiftCounter();
                IncreaseCpuFrequency(bsp::CpuFrequencyMHz::Level_6);
            }
        }
        else {
            if (belowThresholdCounter >= (isFrequencyLoweringInProgress ? powerProfile.maxBelowThresholdInRowCount
                                                                        : powerProfile.maxBelowThresholdCount) &&
                currentCpuFreq > minFrequencyRequested) {
                ResetFrequencyShiftCounter();
                DecreaseCpuFrequency();
            }
        }
    }

    void PowerManager::IncreaseCpuFrequency(bsp::CpuFrequencyMHz newFrequency)
    {
        const auto freq = lowPowerControl->GetCurrentFrequencyLevel();

        if ((freq <= bsp::CpuFrequencyMHz::Level_1) && (newFrequency > bsp::CpuFrequencyMHz::Level_1)) {
            // connect internal the load resistor
            lowPowerControl->ConnectInternalLoadResistor();
            // turn off power save mode for DCDC inverter
            lowPowerControl->DisableDcdcPowerSaveMode();
            // Switch DCDC to full throttle during oscillator switch
            lowPowerControl->SetHighestCoreVoltage();
            // Enable regular 2P5 and 1P1 LDO and Turn off weak 2P5 and 1P1 LDO
            lowPowerControl->SwitchToRegularModeLDO();
            // switch oscillator source
            lowPowerControl->SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource::External);
            // then switch external RAM clock source
            if (driverSEMC) {
                driverSEMC->SwitchToPLL2ClockSource();
            }
            // Add intermediate step in frequency
            if (newFrequency > bsp::CpuFrequencyMHz::Level_4)
                SetCpuFrequency(bsp::CpuFrequencyMHz::Level_4);
        }

        // and increase frequency
        if (freq < newFrequency) {
            SetCpuFrequency(newFrequency);
        }
    }

    void PowerManager::DecreaseCpuFrequency()
    {
        const auto freq = lowPowerControl->GetCurrentFrequencyLevel();
        auto level      = powerProfile.minimalFrequency;

        switch (freq) {
        case bsp::CpuFrequencyMHz::Level_6:
            level = bsp::CpuFrequencyMHz::Level_5;
            break;
        case bsp::CpuFrequencyMHz::Level_5:
            level = bsp::CpuFrequencyMHz::Level_4;
            break;
        case bsp::CpuFrequencyMHz::Level_4:
            level = bsp::CpuFrequencyMHz::Level_3;
            break;
        case bsp::CpuFrequencyMHz::Level_3:
            level = bsp::CpuFrequencyMHz::Level_2;
            break;
        case bsp::CpuFrequencyMHz::Level_2:
            level = bsp::CpuFrequencyMHz::Level_1;
            break;
        case bsp::CpuFrequencyMHz::Level_1:
            level = powerProfile.minimalFrequency;
            break;
        case bsp::CpuFrequencyMHz::Level_0:
            break;
        }

        // decrease frequency first
        if (level != freq) {
            SetCpuFrequency(level);
        }

        if (level <= bsp::CpuFrequencyMHz::Level_1) {
            // Enable weak 2P5 and 1P1 LDO and Turn off regular 2P5 and 1P1 LDO
            lowPowerControl->SwitchToLowPowerModeLDO();

            // then switch osc source
            lowPowerControl->SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource::Internal);

            // and switch external RAM clock source
            if (driverSEMC) {
                driverSEMC->SwitchToPeripheralClockSource();
            }

            // turn on power save mode for DCDC inverter
            lowPowerControl->EnableDcdcPowerSaveMode();

            // disconnect internal the load resistor
            lowPowerControl->DisconnectInternalLoadResistor();
        }

        isFrequencyLoweringInProgress = true;
    }

    void PowerManager::RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel) const
    {
        if (cpuGovernor->RegisterNewSentinel(newSentinel)) {
            newSentinel->ReadRegistrationData(lowPowerControl->GetCurrentFrequencyLevel(),
                                              cpuGovernor->GetPermanentFrequencyRequested().isActive);
        }
    }

    void PowerManager::RemoveSentinel(std::string sentinelName) const
    {
        cpuGovernor->RemoveSentinel(sentinelName);
    }

    void PowerManager::SetCpuFrequencyRequest(std::string sentinelName,
                                              bsp::CpuFrequencyMHz request,
                                              bool permanentBlock)
    {
        cpuGovernor->SetCpuFrequencyRequest(std::move(sentinelName), request, permanentBlock);
    }

    void PowerManager::ResetCpuFrequencyRequest(std::string sentinelName, bool permanentBlock)
    {
        cpuGovernor->ResetCpuFrequencyRequest(std::move(sentinelName), permanentBlock);
    }

    void PowerManager::SetCpuFrequency(bsp::CpuFrequencyMHz freq)
    {
        UpdateCpuFrequencyMonitor(lowPowerControl->GetCurrentFrequencyLevel());
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

    void PowerManager::UpdateCpuFrequencyMonitor(bsp::CpuFrequencyMHz currentFreq)
    {
        auto ticks     = xTaskGetTickCount();
        auto levelName = currentFreq == powerProfile.minimalFrequency
                             ? lowestLevelName
                             : (currentFreq == bsp::CpuFrequencyMHz::Level_6 ? highestLevelName : middleLevelName);

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
        UpdateCpuFrequencyMonitor(lowPowerControl->GetCurrentFrequencyLevel());

        for (auto &level : cpuFrequencyMonitor) {
            log.append(level.GetName() + ": " + std::to_string(level.GetRuntimePercentage()) + "% ");
        }

        LOG_INFO("%s", log.c_str());
    }

    void PowerManager::SetBootSuccess()
    {
        lowPowerControl->SetBootSuccess();
    }
} // namespace sys
