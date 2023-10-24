// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
#include <Logger.hpp>
#include <Utils.hpp>

namespace sys
{
    namespace
    {
        constexpr auto lowestLevelName{"lowestCpuFrequency"};
        constexpr auto middleLevelName{"middleCpuFrequency"};
        constexpr auto highestLevelName{"highestCpuFrequency"};

        constexpr bsp::CpuFrequencyMHz logDumpFrequencyToHold{bsp::CpuFrequencyMHz::Level_4};
    } // namespace

    CpuFrequencyMonitor::CpuFrequencyMonitor(const std::string &name) : levelName(name)
    {}

    [[nodiscard]] auto CpuFrequencyMonitor::GetName() const noexcept -> std::string
    {
        return levelName;
    }

    [[nodiscard]] auto CpuFrequencyMonitor::GetTotalRuntimePercentage(
        const TickType_t totalTicksIncrease) const noexcept -> std::uint32_t
    {
        return totalTicksIncrease == 0 ? 0 : ((static_cast<std::uint64_t>(totalTicksCount) * 100) / totalTicksIncrease);
    }

    [[nodiscard]] auto CpuFrequencyMonitor::GetPeriodRuntimePercentage(
        const TickType_t periodTicksIncrease) const noexcept -> std::uint32_t
    {
        return periodTicksIncrease == 0
                   ? 0
                   : ((static_cast<std::uint64_t>(utils::computeIncrease(totalTicksCount, lastTotalTicksCount)) * 100) /
                      periodTicksIncrease);
    }

    void CpuFrequencyMonitor::IncreaseTicks(TickType_t ticks)
    {
        totalTicksCount += ticks;
    }

    void CpuFrequencyMonitor::SavePeriodTicks()
    {
        lastTotalTicksCount = totalTicksCount;
    }

    PowerManager::PowerManager(CpuStatistics &cpuStats, TaskStatistics &taskStats)
        : powerProfile{bsp::getPowerProfile()}, cpuStatistics(cpuStats), taskStatistics(taskStats)
    {
        driverSEMC      = drivers::DriverSEMC::Create(drivers::name::ExternalRAM);
        lowPowerControl = bsp::LowPowerMode::Create().value_or(nullptr);
        cpuGovernor     = std::make_unique<CpuGovernor>();
        logSentinel                       = std::make_unique<LogSentinel>(logDumpFrequencyToHold);
        Log::Logger::get().preDumpToFile  = [this]() { logSentinel->HoldMinimumFrequency(); };
        Log::Logger::get().postDumpToFile = [this]() { logSentinel->ReleaseMinimumFrequency(); };

        cpuAlgorithms = std::make_unique<cpu::AlgorithmFactory>();
        cpuAlgorithms->emplace(sys::cpu::AlgoID::ImmediateUpscale, std::make_unique<sys::cpu::ImmediateUpscale>());
        cpuAlgorithms->emplace(sys::cpu::AlgoID::FrequencyStepping,
                               std::make_unique<sys::cpu::FrequencyStepping>(powerProfile));

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

    int32_t PowerManager::RebootToRecovery(RecoveryReason reason)
    {
        switch (reason) {
        case RecoveryReason::FactoryReset:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToRecoveryFactoryReset);
        case RecoveryReason::Recovery:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToRecoveryRecovery);
        case RecoveryReason::Update:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToRecoveryUpdate);
        case RecoveryReason::Backup:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToRecoveryBackup);
        case RecoveryReason::Restore:
            return lowPowerControl->Reboot(bsp::LowPowerMode::RebootType::GoToRecoveryRestore);
        default:
            return -1;
        }
    }

    [[nodiscard]] cpu::UpdateResult PowerManager::UpdateCpuFrequency()
    {
        uint32_t cpuLoad = cpuStatistics.GetPercentageCpuLoad();
        cpu::UpdateResult retval;
        const cpu::AlgorithmData data{
            cpuLoad, lowPowerControl->GetCurrentFrequencyLevel(), GetMinimumCpuFrequencyRequested()};

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

    bool PowerManager::IsCpuPermanentFrequency()
    {
        return cpuAlgorithms->get(sys::cpu::AlgoID::FrequencyHold) != nullptr;
    }

    void PowerManager::SetPermanentFrequency(bsp::CpuFrequencyMHz freq)
    {
        cpuAlgorithms->emplace(sys::cpu::AlgoID::FrequencyHold,
                               std::make_unique<sys::cpu::FrequencyHold>(freq, powerProfile));
    }

    void PowerManager::ResetPermanentFrequency()
    {
        cpuAlgorithms->remove(sys::cpu::AlgoID::FrequencyHold);
    }

    void PowerManager::SetCpuFrequency(bsp::CpuFrequencyMHz freq)
    {
        UpdateCpuFrequencyMonitor(lowPowerControl->GetCurrentFrequencyLevel());
        while (lowPowerControl->GetCurrentFrequencyLevel() != freq) {
            lowPowerControl->SetCpuFrequency(freq);
            logSentinel->UpdateCurrentFrequency(freq);
            cpuGovernor->InformSentinelsAboutCpuFrequencyChange(freq);
        }
    }

    [[nodiscard]] auto PowerManager::GetMinimumCpuFrequencyRequested() const noexcept -> sentinel::View
    {
        const auto governorSentinelsView = cpuGovernor->GetMinimumFrequencyRequested();
        const auto logSentinelView       = logSentinel->GetRequestedFrequency();
        return (logSentinelView.minFrequency > governorSentinelsView.minFrequency) ? logSentinelView
                                                                                   : governorSentinelsView;
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

    void PowerManager::LogPowerManagerStatistics()
    {
        const TickType_t tickCount          = xTaskGetTickCount();
        const TickType_t periodTickIncrease = tickCount - lastLogStatisticsTimestamp;
        UpdateCpuFrequencyMonitor(lowPowerControl->GetCurrentFrequencyLevel());

        std::string log{"Last period (total): "};
        for (auto &level : cpuFrequencyMonitor) {
            log.append(level.GetName() + ": " + std::to_string(level.GetPeriodRuntimePercentage(periodTickIncrease)) +
                       "% (" + std::to_string(level.GetTotalRuntimePercentage(tickCount)) + "%) ");
            level.SavePeriodTicks();
        }
        lastLogStatisticsTimestamp = tickCount;
        LOG_INFO("%s", log.c_str());

        taskStatistics.Update();
        taskStatistics.LogCpuUsage();
        cpuGovernor->PrintActiveSentinels();
    }
} // namespace sys
