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

    [[nodiscard]] cpu::UpdateResult PowerManager::UpdateCpuFrequency()
    {
        uint32_t cpuLoad = cpuStatistics.GetPercentageCpuLoad(); 
        cpu::UpdateResult result;
        cpu::AlgorithmData data {cpuLoad,lowPowerControl->GetCurrentFrequencyLevel(), cpuGovernor->GetMinimumFrequencyRequested()};

        auto _ = gsl::finally([&result, this, data] {
            result.frequencySet = lowPowerControl->GetCurrentFrequencyLevel();
            result.changed      = result.frequencySet > data.curentFrequency   ? sys::cpu::UpdateResult::Result::UpScaled
                                  : result.frequencySet < data.curentFrequency ? sys::cpu::UpdateResult::Result::Downscaled
                                                                         : sys::cpu::UpdateResult::Result::NoChange;
            result.data = data.sentinel;
        });

        auto algorithms = {
            sys::cpu::AlgoID::FrequencyHold, sys::cpu::AlgoID::ImmediateUpscale, sys::cpu::AlgoID::FrequencyStepping};

        for (auto id : algorithms) {
            auto algo = cpuAlgorithms->get(id);
            if (algo != nullptr) {
                if (auto frequencyToSet = algo->calculate(data); frequencyToSet != data.curentFrequency) {
                    result.id = id;
                    SetCpuFrequency(frequencyToSet);
                    for (auto again : algorithms) {
                        if (auto al = cpuAlgorithms->get(again); al != nullptr) {
                            al->reset();
                        }
                    }
                    return result;
                }
            }
        }

        return result;
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
        // TODO:
        // - move other UpdateCpuFrequency usages too
        // - if it's ok to trigger algorithms on leave?
        auto ret = UpdateCpuFrequency();
        cpuStatistics.TrackChange(ret);
    }

    void PowerManager::ResetCpuFrequencyRequest(const std::string &sentinelName)
    {
        cpuGovernor->ResetCpuFrequencyRequest(sentinelName);
        //  TODO - same as above
        auto ret = UpdateCpuFrequency();
        cpuStatistics.TrackChange(ret);
    }

    bool PowerManager::IsCpuPernamentFrequency()
    {
        return cpuAlgorithms->get(sys::cpu::AlgoID::FrequencyHold) != nullptr;
    }

    void PowerManager::SetPernamentFrequency(bsp::CpuFrequencyMHz freq)
    {
        cpuAlgorithms->emplace(sys::cpu::AlgoID::FrequencyHold, std::make_unique<sys::cpu::FrequencyHold>(freq,powerProfile));
    }

    void PowerManager::ResetPernamentFrequency()
    {
        cpuAlgorithms->remove(sys::cpu::AlgoID::FrequencyHold);
    }

    void PowerManager::SetCpuFrequency(bsp::CpuFrequencyMHz freq)
    {
       UpdateCpuFrequencyMonitor(lowPowerControl->GetCurrentFrequencyLevel());
        while ( lowPowerControl->GetCurrentFrequencyLevel() != freq ) {
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
