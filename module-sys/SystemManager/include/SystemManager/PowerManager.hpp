// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>

#include "bsp/lpm/bsp_lpm.hpp"
#include "drivers/semc/DriverSEMC.hpp"
#include "SysCpuUpdateResult.hpp"
#include "CpuGovernor.hpp"
#include "LogSentinel.hpp"
#include "TaskStatistics.hpp"
#include <bsp/lpm/PowerProfile.hpp>
#include <vector>

namespace sys::cpu
{
    class AlgorithmFactory;
}

namespace sys
{
    class CpuStatistics;

    class CpuFrequencyMonitor
    {
      public:
        explicit CpuFrequencyMonitor(const std::string &name);

        [[nodiscard]] auto GetName() const noexcept -> std::string;
        [[nodiscard]] auto GetPeriodRuntimePercentage(TickType_t periodTicksIncrease) const noexcept -> std::uint32_t;
        [[nodiscard]] auto GetTotalRuntimePercentage(TickType_t totalTicksIncrease) const noexcept -> std::uint32_t;
        void IncreaseTicks(TickType_t ticks);
        void SavePeriodTicks();

      private:
        std::string levelName;
        std::uint32_t totalTicksCount{0};
        std::uint32_t lastTotalTicksCount{0};
    };

    class PowerManager
    {
      public:
        PowerManager(CpuStatistics &cpuStats, TaskStatistics &taskStats);
        ~PowerManager();

        std::int32_t PowerOff();
        std::int32_t Reboot();
        std::int32_t RebootMSC();
        std::int32_t RebootToRecovery(RecoveryReason reason);

        /// called periodically to calculate the CPU requirement
        ///
        /// if for the last 'maxAboveThresholdCount' periods the current CPU consumption has been above the set upper
        /// limit (frequencyShiftUpperThreshold), CPU frequency is increased; if for the last 'maxBelowThresholdCount'
        /// periods the current CPU usage was below the lower limit (frequencyShiftLowerThreshold), CPU frequency is
        /// reduced frequency
        [[nodiscard]] cpu::UpdateResult UpdateCpuFrequency();

        [[nodiscard]] auto getExternalRamDevice() const noexcept -> std::shared_ptr<devices::Device>;

        void RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel) const;
        void RemoveSentinel(std::string sentinelName) const;
        void SetCpuFrequencyRequest(const std::string &sentinelName, bsp::CpuFrequencyMHz request);
        void ResetCpuFrequencyRequest(const std::string &sentinelName);
        bool IsCpuPermanentFrequency();
        void SetPermanentFrequency(bsp::CpuFrequencyMHz freq);
        void ResetPermanentFrequency();
        void BlockWfiMode(const std::string &sentinelName, bool block);
        void EnterWfiIfReady();
        void LogPowerManagerStatistics();

      private:
        void SetCpuFrequency(bsp::CpuFrequencyMHz freq);
        void UpdateCpuFrequencyMonitor(bsp::CpuFrequencyMHz currentFreq);
        void UpdateCpuFrequencyMonitor(const std::string &name, std::uint32_t tickIncrease);
        void UpdateCpuFrequencyMonitorTimestamp();
        [[nodiscard]] auto GetMinimumCpuFrequencyRequested() const noexcept -> sentinel::View;

        TickType_t lastCpuFrequencyChangeTimestamp{0};
        TickType_t lastLogStatisticsTimestamp{0};

        std::vector<CpuFrequencyMonitor> cpuFrequencyMonitors;

        std::shared_ptr<drivers::DriverSEMC> driverSEMC;
        std::unique_ptr<bsp::LowPowerMode> lowPowerControl;
        std::unique_ptr<CpuGovernor> cpuGovernor;
        std::unique_ptr<LogSentinel> logSentinel;
        const bsp::PowerProfile powerProfile;

        std::unique_ptr<sys::cpu::AlgorithmFactory> cpuAlgorithms;
        CpuStatistics &cpuStatistics;
        TaskStatistics &taskStatistics;
    };
} // namespace sys
