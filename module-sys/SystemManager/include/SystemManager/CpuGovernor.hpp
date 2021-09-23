// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <vector>
#include "SystemManager/CpuSentinel.hpp"

namespace sys
{
    using SentinelPointer = std::weak_ptr<CpuSentinel>;

    class GovernorSentinel
    {
      public:
        explicit GovernorSentinel(std::shared_ptr<CpuSentinel> newSentinel);
        [[nodiscard]] auto GetSentinel() const noexcept -> SentinelPointer;
        [[nodiscard]] auto GetRequestedFrequency() const noexcept -> bsp::CpuFrequencyHz;
        void SetRequestedFrequency(bsp::CpuFrequencyHz newFrequency);

      private:
        SentinelPointer sentinelPtr;
        bsp::CpuFrequencyHz requestedFrequency;
    };

    using GovernorSentinelPointer = std::unique_ptr<GovernorSentinel>;
    using GovernorSentinelsVector = std::vector<GovernorSentinelPointer>;

    /// CpuGovernor manages all sentinels in the system and has CPU frequency requests from them (e.g. eInkSentinel).
    /// It is also responsible for informing all sentinels that the CPU frequency has changed.
    class CpuGovernor
    {

      public:
        void RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel);
        [[nodiscard]] auto GetNumberOfRegisteredSentinels() const noexcept -> uint32_t;
        void PrintAllSentinels() const noexcept;

        void SetCpuFrequencyRequest(std::string sentinelName, bsp::CpuFrequencyHz request);
        void ResetCpuFrequencyRequest(std::string sentinelName);

        [[nodiscard]] auto GetMinimumFrequencyRequested() const noexcept -> bsp::CpuFrequencyHz;
        void InformSentinelsAboutCpuFrequencyChange(bsp::CpuFrequencyHz newFrequency) const noexcept;

      private:
        static void PrintName(const GovernorSentinelPointer &element);

        GovernorSentinelsVector sentinels;
    };

} // namespace sys
