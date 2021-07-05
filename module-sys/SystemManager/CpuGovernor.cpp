// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CpuGovernor.hpp"
#include <algorithm>
#include <log.hpp>

namespace sys
{

    GovernorSentinel::GovernorSentinel(std::shared_ptr<CpuSentinel> newSentinel)
        : sentinelPtr(newSentinel), requestedFrequency(bsp::CpuFrequencyHz::Level_1)
    {}

    [[nodiscard]] auto GovernorSentinel::GetSentinel() const noexcept -> SentinelPointer
    {
        return sentinelPtr;
    }

    [[nodiscard]] auto GovernorSentinel::GetRequestedFrequency() const noexcept -> bsp::CpuFrequencyHz
    {
        return requestedFrequency;
    }

    void GovernorSentinel::SetRequestedFrequency(bsp::CpuFrequencyHz newFrequency)
    {
        requestedFrequency = newFrequency;
    }

    void CpuGovernor::RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel)
    {
        if (newSentinel) {
            auto isNewSentinelAlreadyRegistered = false;

            for (auto &sentinel : sentinels) {
                auto sentinelWeakPointer = sentinel->GetSentinel();
                if (!sentinelWeakPointer.expired()) {
                    std::shared_ptr<CpuSentinel> sharedResource = sentinelWeakPointer.lock();
                    if (sharedResource->GetName() == newSentinel->GetName()) {
                        isNewSentinelAlreadyRegistered = true;
                        break;
                    }
                }
            }

            if (!isNewSentinelAlreadyRegistered) {
                sentinels.push_back(std::make_unique<GovernorSentinel>(newSentinel));
            }
            else {
                LOG_WARN("New sentinel %s is already registered", newSentinel->GetName().c_str());
            }
        }
    }

    [[nodiscard]] auto CpuGovernor::GetNumberOfRegisteredSentinels() const noexcept -> uint32_t
    {
        return sentinels.size();
    }

    void CpuGovernor::PrintAllSentinels() const noexcept
    {
        std::for_each(std::begin(sentinels), std::end(sentinels), PrintName);
    }

    void CpuGovernor::SetCpuFrequencyRequest(std::string sentinelName, bsp::CpuFrequencyHz request)
    {
        for (auto &sentinel : sentinels) {
            auto sentinelWeakPointer = sentinel->GetSentinel();
            if (!sentinelWeakPointer.expired()) {
                std::shared_ptr<CpuSentinel> sharedResource = sentinelWeakPointer.lock();
                if (sharedResource->GetName() == sentinelName) {
                    sentinel->SetRequestedFrequency(request);
                }
            }
        }
    }

    void CpuGovernor::ResetCpuFrequencyRequest(std::string sentinelName)
    {
        SetCpuFrequencyRequest(sentinelName, bsp::CpuFrequencyHz::Level_1);
    }

    [[nodiscard]] auto CpuGovernor::GetMinimumFrequencyRequested() const noexcept -> bsp::CpuFrequencyHz
    {
        bsp::CpuFrequencyHz minFrequency = bsp::CpuFrequencyHz::Level_1;

        for (auto &sentinel : sentinels) {
            const auto sentinelFrequency = sentinel->GetRequestedFrequency();

            if (sentinelFrequency > minFrequency) {
                minFrequency = sentinelFrequency;
            }
        }

        return minFrequency;
    }

    void CpuGovernor::InformSentinelsAboutCpuFrequencyChange(bsp::CpuFrequencyHz newFrequency) const noexcept
    {
        for (auto &sentinel : sentinels) {
            auto sentinelWeakPointer = sentinel->GetSentinel();
            if (!sentinelWeakPointer.expired()) {
                std::shared_ptr<CpuSentinel> sharedResource = sentinelWeakPointer.lock();
                sharedResource->CpuFrequencyHasChanged(newFrequency);
            }
        }
    }

    void CpuGovernor::PrintName(const GovernorSentinelPointer &element)
    {
        auto sentinelWeakPointer = element->GetSentinel();
        if (!sentinelWeakPointer.expired()) {
            std::shared_ptr<CpuSentinel> sharedResource = sentinelWeakPointer.lock();
            LOG_INFO("Sentinel %s", sharedResource->GetName().c_str());
        }
    }

} // namespace sys
