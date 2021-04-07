// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/popups/lock-widgets/PinLock.hpp"

namespace gui
{
    class ChangePasscodeLockHandler
    {
      public:
        ChangePasscodeLockHandler();

        auto checkPasscode(unsigned int currentLockPassHash) -> PinLock::LockState;
        [[nodiscard]] auto getLock() -> PinLock &
        {
            return lock;
        }
        [[nodiscard]] auto getNewPasscodeHash() const noexcept -> unsigned int
        {
            return newPasscodeHash;
        }
        auto newPasscodeConfirmed() -> PinLock::LockState;
        auto newPasscodeProvided() -> PinLock::LockState;

      private:
        using OnActivatedCallback = std::function<void(PinLock::LockType type, const std::vector<unsigned int> &)>;
        auto activateLock(OnActivatedCallback onActivatedCallback) -> PinLock::LockState;

        PinLock lock;
        unsigned int newPasscodeHash;
    };
} // namespace gui
