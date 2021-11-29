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
        [[nodiscard]] auto GetRequestedFrequency() const noexcept -> bsp::CpuFrequencyMHz;
        void SetRequestedFrequency(bsp::CpuFrequencyMHz newFrequency);

      private:
        SentinelPointer sentinelPtr;
        bsp::CpuFrequencyMHz requestedFrequency;
    };

    using GovernorSentinelPointer = std::unique_ptr<GovernorSentinel>;
    using GovernorSentinelsVector = std::vector<GovernorSentinelPointer>;

    /// CpuGovernor manages all sentinels in the system and has CPU frequency requests from them (e.g. eInkSentinel).
    /// It is also responsible for informing all sentinels that the CPU frequency has changed.
    class CpuGovernor
    {

      public:
        auto RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel) -> bool;
        auto RemoveSentinel(std::string sentinelName) -> void;
        [[nodiscard]] auto GetNumberOfRegisteredSentinels() const noexcept -> uint32_t;
        void PrintAllSentinels() const noexcept;

        void SetCpuFrequencyRequest(std::string sentinelName,
                                    bsp::CpuFrequencyMHz request,
                                    bool permanentBlock = false);
        void ResetCpuFrequencyRequest(std::string sentinelName, bool permanentBlock = false);

        [[nodiscard]] auto GetMinimumFrequencyRequested() const noexcept -> bsp::CpuFrequencyMHz;
        void InformSentinelsAboutCpuFrequencyChange(bsp::CpuFrequencyMHz newFrequency) const noexcept;

        [[nodiscard]] auto GetPermanentFrequencyRequested() const noexcept -> PermanentFrequencyToHold;

      private:
        static void PrintName(const GovernorSentinelPointer &element);

        GovernorSentinelsVector sentinels;
        PermanentFrequencyToHold permanentFrequencyToHold{false, bsp::CpuFrequencyMHz::Level_0};
    };

} // namespace sys
