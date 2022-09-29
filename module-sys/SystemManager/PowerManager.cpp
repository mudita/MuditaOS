// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemManager/cpu/algorithm/FrequencyHold.hpp"
#include "SystemManager/cpu/algorithm/ImmediateUpscale.hpp"
#include "SystemManager/cpu/algorithm/FrequencyStepping.hpp"
#include "cpu/AlgorithmFactory.hpp"
#include "magic_enum.hpp"
#include <SystemManager/CpuStatistics.hpp>
#include <SystemManager/PowerManager.hpp>
#include <gsl/util>
#include <log/log.hpp>

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

    PowerManager::PowerManager(CpuStatistics &stats) : powerProfile{bsp::getPowerProfile()}, cpuStatistics(stats)
    {
        driverSEMC      = drivers::DriverSEMC::Create(drivers::name::ExternalRAM);
        lowPowerControl = bsp::LowPowerMode::Create().value_or(nullptr);
        cpuGovernor     = std::make_unique<CpuGovernor>();

        cpuAlgorithms = std::make_unique<cpu::AlgorithmFactory>();
        cpuAlgorithms->emplace(sys::cpu::AlgoID::ImmediateUpscale, std::make_unique<sys::cpu::ImmediateUpscale>());
        cpuAlgorithms->emplace(sys::cpu::AlgoID::FrequencyStepping,
                               std::make_unique<sys::cpu::FrequencyStepping>(powerProfile, *cpuGovernor));

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

    int32_t PowerManager::RebootToUpdater(UpdaterReason reason)
    {
        switch (reason) {
        case UpdaterReason::FactoryReset:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToUpdaterFactoryReset);
        case UpdaterReason::Recovery:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToUpdaterRecovery);
        case UpdaterReason::Update:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToUpdaterUpdate);
        case UpdaterReason::Backup:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToUpdaterBackup);
        case UpdaterReason::Restore:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToUpdaterRestore);
        default:
            return -1;
        }
    }

    [[nodiscard]] cpu::UpdateResult PowerManager::UpdateCpuFrequency()
    {
        uint32_t cpuLoad = cpuStatistics.GetPercentageCpuLoad();
        cpu::UpdateResult retval;
        cpu::AlgorithmData data{
            cpuLoad, lowPowerControl->GetCurrentFrequencyLevel(), cpuGovernor->GetMinimumFrequencyRequested()};

        auto _ = gsl::finally([&retval, this, data] {
            retval.frequencySet = lowPowerControl->GetCurrentFrequencyLevel();
            retval.data         = data.sentinel;
        });

        auto algorithms = {
            sys::cpu::AlgoID::FrequencyHold, sys::cpu::AlgoID::ImmediateUpscale, sys::cpu::AlgoID::FrequencyStepping};

        auto result    = cpuAlgorithms->calculate(algorithms, data, &retval.id);
        retval.changed = result.change;
        if (result.change == cpu::algorithm::Change::NoChange or result.change == cpu::algorithm::Change::Hold) {
            return retval;
        }
        SetCpuFrequency(result.value);
        cpuAlgorithms->reset(algorithms);
        return retval;
    }

    void PowerManager::RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel) const
    {
        if (cpuGovernor->RegisterNewSentinel(newSentinel)) {
            newSentinel->ReadRegistrationData(lowPowerControl->GetCurrentFrequencyLevel());
        }
    }

    void PowerManager::RemoveSentinel(std::string sentinelName) const
    {
        cpuGovernor->RemoveSentinel(sentinelName);
    }

    void PowerManager::SetCpuFrequencyRequest(const std::string &sentinelName, bsp::CpuFrequencyMHz request)
    {
        cpuGovernor->SetCpuFrequencyRequest(sentinelName, request);
        auto ret = UpdateCpuFrequency();
        cpuStatistics.TrackChange(ret);
    }

    void PowerManager::ResetCpuFrequencyRequest(const std::string &sentinelName)
    {
        cpuGovernor->ResetCpuFrequencyRequest(sentinelName);
        auto ret = UpdateCpuFrequency();
        cpuStatistics.TrackChange(ret);
    }

    bool PowerManager::IsCpuPernamentFrequency()
    {
        return cpuAlgorithms->get(sys::cpu::AlgoID::FrequencyHold) != nullptr;
    }

    void PowerManager::SetPernamentFrequency(bsp::CpuFrequencyMHz freq)
    {
        cpuAlgorithms->emplace(sys::cpu::AlgoID::FrequencyHold,
                               std::make_unique<sys::cpu::FrequencyHold>(freq, powerProfile));
    }

    void PowerManager::ResetPernamentFrequency()
    {
        cpuAlgorithms->remove(sys::cpu::AlgoID::FrequencyHold);
    }

    void PowerManager::SetCpuFrequency(bsp::CpuFrequencyMHz freq)
    {
        UpdateCpuFrequencyMonitor(lowPowerControl->GetCurrentFrequencyLevel());
        while (lowPowerControl->GetCurrentFrequencyLevel() != freq) {
            lowPowerControl->SetCpuFrequency(freq);
            cpuGovernor->InformSentinelsAboutCpuFrequencyChange(freq);
        }
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
