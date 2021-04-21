// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/popups/lock-widgets/Lock.hpp"

namespace gui
{
    using namespace lock;

    class ChangePasscodeLockHandler
    {
      public:
        ChangePasscodeLockHandler();

        auto checkPasscode(unsigned int currentLockPassHash) -> Lock::LockState;
        [[nodiscard]] auto getLock() -> Lock &
        {
            return lock;
        }
        [[nodiscard]] auto getNewPasscodeHash() const noexcept -> unsigned int
        {
            return newPasscodeHash;
        }
        auto newPasscodeConfirmed() -> Lock::LockState;
        auto newPasscodeProvided() -> Lock::LockState;

      private:
        using OnActivatedCallback = std::function<void(Lock::LockType type, const std::vector<unsigned int> &)>;
        auto activateLock(OnActivatedCallback onActivatedCallback) -> Lock::LockState;

        Lock lock;
        unsigned int newPasscodeHash;
    };
} // namespace gui
