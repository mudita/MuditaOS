// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>

#include "bsp/lpm/bsp_lpm.hpp"
#include "drivers/semc/DriverSEMC.hpp"
#include "SysCpuUpdateResult.hpp"
#include "CpuGovernor.hpp"
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
        explicit CpuFrequencyMonitor(const std::string name);

        [[nodiscard]] auto GetName() const noexcept -> std::string;
        [[nodiscard]] auto GetPeriodRuntimePercentage(const TickType_t periodTicksIncrease) const noexcept
            -> std::uint32_t;
        [[nodiscard]] auto GetTotalRuntimePercentage(const TickType_t totalTicksIncrease) const noexcept
            -> std::uint32_t;
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
        explicit PowerManager(CpuStatistics &cpuStats, TaskStatistics &taskStats);
        ~PowerManager();

        int32_t PowerOff();
        int32_t Reboot();
        int32_t RebootToRecovery(RecoveryReason reason);

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
        bool IsCpuPernamentFrequency();
        void SetPernamentFrequency(bsp::CpuFrequencyMHz freq);
        void ResetPernamentFrequency();

        void LogPowerManagerStatistics();

      private:
        void SetCpuFrequency(bsp::CpuFrequencyMHz freq);

        void UpdateCpuFrequencyMonitor(bsp::CpuFrequencyMHz currentFreq);

        TickType_t lastCpuFrequencyChangeTimestamp{0};
        TickType_t lastLogStatisticsTimestamp{0};

        std::vector<CpuFrequencyMonitor> cpuFrequencyMonitor;

        std::shared_ptr<drivers::DriverSEMC> driverSEMC;
        std::unique_ptr<bsp::LowPowerMode> lowPowerControl;
        std::unique_ptr<CpuGovernor> cpuGovernor;
        const bsp::PowerProfile powerProfile;

        std::unique_ptr<sys::cpu::AlgorithmFactory> cpuAlgorithms;
        CpuStatistics &cpuStatistics;
        TaskStatistics &taskStatistics;
    };

} // namespace sys
