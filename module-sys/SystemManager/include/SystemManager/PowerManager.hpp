// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_POWERMANAGER_HPP
#define PUREPHONE_POWERMANAGER_HPP

#include <functional>

#include "bsp/lpm/bsp_lpm.hpp"
#include "drivers/semc/DriverSEMC.hpp"
#include "CpuGovernor.hpp"
#include <vector>

namespace sys
{
    class CpuFrequencyMonitor
    {
      public:
        explicit CpuFrequencyMonitor(const std::string name);

        [[nodiscard]] auto GetName() const noexcept -> std::string;
        [[nodiscard]] auto GetRuntimePercentage() const noexcept -> std::uint32_t;
        void IncreaseTicks(TickType_t ticks);

      private:
        std::string levelName;
        TickType_t totalTicksCount{0};
    };

    class PowerManager
    {

      public:
        PowerManager();
        ~PowerManager();

        int32_t PowerOff();
        int32_t Reboot();
        int32_t RebootToUpdate(UpdateReason reason);

        /// called periodically to calculate the CPU requirement
        ///
        /// if for the last 'maxAboveThresholdCount' periods the current CPU consumption has been above the set upper
        /// limit (frequencyShiftUpperThreshold), CPU frequency is increased; if for the last 'maxBelowThresholdCount'
        /// periods the current CPU usage was below the lower limit (frequencyShiftLowerThreshold), CPU frequency is
        /// reduced frequency
        /// @param current cpu load
        void UpdateCpuFrequency(uint32_t cpuLoad);

        [[nodiscard]] auto getExternalRamDevice() const noexcept -> std::shared_ptr<devices::Device>;

        void RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel) const;
        void SetCpuFrequencyRequest(std::string sentinelName, bsp::CpuFrequencyHz request);
        void ResetCpuFrequencyRequest(std::string sentinelName);
        void LogPowerManagerEfficiency();
        void SetBootSuccess();

      private:
        /// called when the CPU frequency needs to be increased
        void IncreaseCpuFrequency(bsp::CpuFrequencyHz newFrequency);

        /// called when the CPU frequency needs to be reduced
        /// @note the frequency is always reduced by one step
        void DecreaseCpuFrequency();

        void ResetFrequencyShiftCounter();
        void SetCpuFrequency(bsp::CpuFrequencyHz freq);

        void UpdateCpuFrequencyMonitor(bsp::CpuFrequencyHz currentFreq);

        uint32_t belowThresholdCounter{0};
        uint32_t aboveThresholdCounter{0};
        TickType_t lastCpuFrequencyChangeTimestamp{0};
        bool isFrequencyLoweringInProgress{false};

        std::vector<CpuFrequencyMonitor> cpuFrequencyMonitor;

        std::unique_ptr<bsp::LowPowerMode> lowPowerControl;
        std::shared_ptr<drivers::DriverSEMC> driverSEMC;
        std::unique_ptr<CpuGovernor> cpuGovernor;
    };

} // namespace sys

#endif // PUREPHONE_POWERMANAGER_HPP
