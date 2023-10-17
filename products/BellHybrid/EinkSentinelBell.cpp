// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkSentinel.hpp"

#include <bsp/common.hpp>

namespace service::eink
{
    namespace
    {
        constexpr auto RedrawEinkCpuFrequency = bsp::CpuFrequencyMHz::Level_4;
    } // namespace

    EinkSentinel::EinkSentinel(std::string name, sys::Service *service) : sys::CpuSentinel(name, service)
    {}

    void EinkSentinel::HoldMinimumFrequency()
    {
        CpuSentinel::HoldMinimumFrequency(RedrawEinkCpuFrequency);
    }

    sys::MessagePointer EinkSentinel::handleLockedPhone()
    {
        return sys::MessageNone{};
    }

    sys::MessagePointer EinkSentinel::handleUnlockedPhone()
    {
        return sys::MessageNone{};
    }

} // namespace service::eink
