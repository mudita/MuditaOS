// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkSentinel.hpp"

#include <apps-common/locks/data/PhoneLockMessages.hpp>
#include <bsp/common.hpp>

namespace service::eink
{
    namespace
    {
        constexpr auto RedrawLockedEinkCpuFrequency   = bsp::CpuFrequencyHz::Level_3;
        constexpr auto RedrawUnlockedEinkCpuFrequency = bsp::CpuFrequencyHz::Level_6;
    } // namespace

    EinkSentinel::EinkSentinel(std::string name, sys::Service *service) : sys::CpuSentinel(name, service)
    {
        owner->bus.channels.push_back(sys::BusChannel::PhoneLockChanges);

        owner->connect(typeid(locks::UnlockedPhone),
                       [this](sys::Message *msg) -> sys::MessagePointer { return handleUnlockedPhone(); });

        owner->connect(typeid(locks::LockedPhone),
                       [this](sys::Message *msg) -> sys::MessagePointer { return handleLockedPhone(); });
    }

    void EinkSentinel::HoldMinimumFrequency()
    {
        CpuSentinel::HoldMinimumFrequency(isScreenLocked ? RedrawLockedEinkCpuFrequency
                                                         : RedrawUnlockedEinkCpuFrequency);
    }

    sys::MessagePointer EinkSentinel::handleLockedPhone()
    {
        isScreenLocked = true;
        return sys::MessageNone{};
    }

    sys::MessagePointer EinkSentinel::handleUnlockedPhone()
    {
        isScreenLocked = false;
        return sys::MessageNone{};
    }

} // namespace service::eink
