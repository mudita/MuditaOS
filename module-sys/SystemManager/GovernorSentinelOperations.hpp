// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <SystemManager/CpuGovernor.hpp>

namespace sys
{
    /// return true if break of for each is neeed, false othervise
    using governed_callback = std::function<bool(const GovernorSentinel &s)>;

    /// loop over all Sentinel governors (in which sentinels are in)
    /// - will remove tracking non existent sentinels upon iteration over
    /// - returns on first sentinel_foo callback with 'true' result
    void for_each_governed_sentinel(GovernorSentinelsVector &sentinels, governed_callback &foo);

    /// return true if break of for each is neeed, false othervise
    using sentinel_foo = std::function<bool(const std::shared_ptr<CpuSentinel> &s)>;

    /// loop over all sentinels:
    /// - uses for_each_governed_sentinel
    /// - may remove tracking non existent sentinels
    /// - returns on first sentinel_foo callback with 'true' result
    void for_each_sentinel(GovernorSentinelsVector &sentinels, sentinel_foo &foo);

    /// removes all non existent sentinels from GovernorSentinelsVector
    void clean(GovernorSentinelsVector &sentinels);
} // namespace sys
