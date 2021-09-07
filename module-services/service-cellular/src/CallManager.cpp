// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallManager.hpp"

namespace cellular::internal
{
    void SimpleCallManager::hangUp() noexcept
    {
        incomingCallPropagated = false;
        callerInfoComplete     = false;
    }

    void SimpleCallManager::ring() noexcept
    {
        incomingCallPropagated = true;
    }

    void SimpleCallManager::completeCallerInfo() noexcept
    {
        callerInfoComplete = true;
    }

    auto SimpleCallManager::isIncomingCallPropagated() const noexcept -> bool
    {
        return incomingCallPropagated;
    }

    auto SimpleCallManager::isCallerInfoComplete() const noexcept -> bool
    {
        return callerInfoComplete;
    }
} // namespace cellular::internal
