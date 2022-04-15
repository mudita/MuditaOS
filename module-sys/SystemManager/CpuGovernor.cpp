// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/CpuGovernor.hpp>
#include <algorithm>
#include <log/log.hpp>
#include "GovernorSentinelOperations.hpp"
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
            sentinel_foo checkSentinelAlreadyRegistered = [&](const std::shared_ptr<CpuSentinel> &s) -> bool {
                if (s->GetName() == newSentinel->GetName()) {
                    isNewSentinelAlreadyRegistered = true;
                    return true;
                }
                return false;
            };

            for_each_sentinel(sentinels, checkSentinelAlreadyRegistered);

            if (!isNewSentinelAlreadyRegistered) {
                sentinels.push_back(std::make_unique<GovernorSentinel>(newSentinel));
                return true;
            }
            LOG_WARN("New sentinel %s is already registered", newSentinel->GetName().c_str());
            return false;
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

    void CpuGovernor::SetCpuFrequencyRequest(const std::string &sentinelName, bsp::CpuFrequencyMHz request)
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

    void CpuGovernor::ResetCpuFrequencyRequest(const std::string &sentinelName)
    {
        SetCpuFrequencyRequest(sentinelName, bsp::CpuFrequencyMHz::Level_0);
    }

    [[nodiscard]] auto CpuGovernor::GetMinimumFrequencyRequested() noexcept -> sentinel::View
    {
        sentinel::View d;
        if (sentinels.empty()) {
            d.reason = "empty";
            return d;
        }

        clean(sentinels);

        auto minSentinel = std::max_element(sentinels.begin(), sentinels.end(), [](const auto &l, const auto &r) {
            return (*l).GetRequestedFrequency() < (*r).GetRequestedFrequency();
        });

        d.frequency = (*minSentinel)->GetRequestedFrequency();
        if (auto p = (*minSentinel)->GetSentinel().lock()) {
            d.name   = p->GetName();
            d.reason = p->getReason();
        }
        else {
            d.reason = "cant lock";
        }
        return d;
    }

    void CpuGovernor::InformSentinelsAboutCpuFrequencyChange(bsp::CpuFrequencyMHz newFrequency) noexcept
    {
        sentinel_foo foo = [&newFrequency](const std::shared_ptr<CpuSentinel> &s) -> bool {
            s->CpuFrequencyHasChanged(newFrequency);
            return false;
        };

        for_each_sentinel(sentinels, foo);
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
