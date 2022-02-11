// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/CpuGovernor.hpp>
#include <algorithm>
#include <log/log.hpp>

namespace sys
{

    GovernorSentinel::GovernorSentinel(std::shared_ptr<CpuSentinel> newSentinel)
        : sentinelPtr(newSentinel), requestedFrequency(bsp::CpuFrequencyMHz::Level_0)
    {}

    [[nodiscard]] auto GovernorSentinel::GetSentinel() const noexcept -> SentinelPointer
    {
        return sentinelPtr;
    }

    [[nodiscard]] auto GovernorSentinel::GetRequestedFrequency() const noexcept -> bsp::CpuFrequencyMHz
    {
        return requestedFrequency;
    }

    void GovernorSentinel::SetRequestedFrequency(bsp::CpuFrequencyMHz newFrequency)
    {
        requestedFrequency = newFrequency;
    }

    bool CpuGovernor::RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel)
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
                return true;
            }
            else {
                return false;
                LOG_WARN("New sentinel %s is already registered", newSentinel->GetName().c_str());
            }
        }
        return false;
    }

    void CpuGovernor::RemoveSentinel(std::string sentinelName)
    {
        if (!sentinelName.empty()) {
            sentinels.erase(std::remove_if(sentinels.begin(),
                                           sentinels.end(),
                                           [sentinelName](auto const &sentinel) {
                                               auto sentinelWeakPointer = sentinel->GetSentinel();
                                               if (!sentinelWeakPointer.expired()) {
                                                   std::shared_ptr<CpuSentinel> sharedResource =
                                                       sentinelWeakPointer.lock();
                                                   return sharedResource->GetName() == sentinelName;
                                               }
                                               return false;
                                           }),
                            sentinels.end());
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

    void CpuGovernor::SetCpuFrequencyRequest(std::string sentinelName,
                                             bsp::CpuFrequencyMHz request,
                                             bool permanentBlock)
    {
        if (permanentBlock) {
            permanentFrequencyToHold.isActive        = true;
            permanentFrequencyToHold.frequencyToHold = request;
            return;
        }
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

    void CpuGovernor::ResetCpuFrequencyRequest(std::string sentinelName, bool permanentBlock)
    {
        if (permanentBlock) {
            permanentFrequencyToHold.isActive        = false;
            permanentFrequencyToHold.frequencyToHold = bsp::CpuFrequencyMHz::Level_0;
            return;
        }
        SetCpuFrequencyRequest(sentinelName, bsp::CpuFrequencyMHz::Level_0);
    }

    [[nodiscard]] auto CpuGovernor::GetMinimumFrequencyRequested() const noexcept -> sentinel::Data
    {
        sentinel::Data d;
        if (sentinels.empty()) {
            d.reason = "empty";
            return d;
        }

        auto minSentinel = sentinels.begin();
        for (auto iter = sentinels.begin(); iter != std::end(sentinels); ++iter) {
            const auto sentinelFrequency = (*iter)->GetRequestedFrequency();

            if (sentinelFrequency > (*minSentinel)->GetRequestedFrequency()) {
                minSentinel = iter;
            }
        }

        d.frequency = (*minSentinel)->GetRequestedFrequency();
        if (auto p = (*minSentinel)->GetSentinel().lock()) {
            d.name   = p->GetName();
            d.task   = p->getTask();
            d.reason = p->getReason();
        }
        else {
            d.reason = "cant lock";
        }
        return d;
    }

    void CpuGovernor::InformSentinelsAboutCpuFrequencyChange(bsp::CpuFrequencyMHz newFrequency) const noexcept
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

    [[nodiscard]] auto CpuGovernor::GetPermanentFrequencyRequested() const noexcept -> PermanentFrequencyToHold
    {
        return permanentFrequencyToHold;
    }

} // namespace sys
