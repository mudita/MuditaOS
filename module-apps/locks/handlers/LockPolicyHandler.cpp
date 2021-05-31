// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LockPolicyHandler.hpp"
#include <log.hpp>
#include <Application.hpp>
#include <gsl/assert>

using namespace locks;

AutoLockPolicy LockPolicyAccessInterface::get() const noexcept
{
    return autoLockingPolicy;
}

void LockPolicyAccessInterface::set(AutoLockPolicy policy) noexcept
{
    if (autoLockingPolicy != AutoLockPolicy::PreventPermanently) {
        autoLockingPolicy = policy;
    }
    else {
        LOG_ERROR("AutoLocking is prevented permanently");
    }
}

bool LockPolicyHandlerInterface::preventsAutoLocking()
{
    if (const auto policy = get(); policy == AutoLockPolicy::DetermineByWindow) {
        return preventsAutoLockByWindow();
    }
    else if (policy == AutoLockPolicy::DetermineByAppState) {
        return preventsAutoLockByState() || preventsAutoLockByWindow();
    }
    return true;
}

LockPolicyHandler::LockPolicyHandler(app::Application *owner,
                                     std::function<bool()> preventsAutoLockByStateCallback) noexcept
    : owner{owner}, preventsAutoLockByStateCallback(std::move(preventsAutoLockByStateCallback))
{
    Expects(owner != nullptr);
}

void LockPolicyHandler::setPreventsAutoLockByStateCallback(
    std::function<bool()> _preventsAutoLockByStateCallback) noexcept
{
    preventsAutoLockByStateCallback = std::move(_preventsAutoLockByStateCallback);
}
bool LockPolicyHandler::preventsAutoLockByWindow()
{
    return owner->getCurrentWindow()->preventsAutoLocking();
}
bool LockPolicyHandler::preventsAutoLockByState() const
{
    Expects(preventsAutoLockByStateCallback != nullptr);
    return preventsAutoLockByStateCallback();
}
