// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "GovernorSentinelOperations.hpp"

namespace sys
{

    void for_each_governed_sentinel(GovernorSentinelsVector &sentinels, governed_callback &foo)
    {
        for (auto sentinel = sentinels.rbegin(); sentinels.rend() != sentinel; ++sentinel) {
            // remove dangling unique ptrs
            if (not *sentinel) {
                sentinels.erase(sentinel.base());
                continue;
            }
            auto sentinelWeakPointer = (*sentinel)->GetSentinel();
            // remove expired ptrs
            if (sentinelWeakPointer.expired()) {
                // shift iterator to point on the correct successor,
                // without std::next it removes (sentinel - 1) element
                sentinels.erase(std::next(sentinel).base());
                continue;
            }
            // call foo on sentinel
            if (foo(**sentinel)) {
                return;
            };
        }
    };

    void for_each_sentinel(GovernorSentinelsVector &sentinels, sentinel_foo &foo)
    {
        governed_callback cb = [&](const GovernorSentinel &sentinel) {
            std::shared_ptr<CpuSentinel> sharedResource = sentinel.GetSentinel().lock();
            if (sharedResource) {
                if (foo(sharedResource)) {
                    return true;
                }
            }
            return false;
        };
        for_each_governed_sentinel(sentinels, cb);
    };

    void clean(GovernorSentinelsVector &sentinels)
    {
        governed_callback empty_governed_callback = [&](const GovernorSentinel &) { return false; };
        for_each_governed_sentinel(sentinels, empty_governed_callback);
    }
} // namespace sys
