// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "widgets/PinLock.hpp"

namespace gui
{
    class ScreenLockHandler
    {
      public:
        ScreenLockHandler();

        auto checkPasscode(unsigned int currentLockPassHash) -> PinLock::LockState;
        auto newPasscodeProvided() -> PinLock::LockState;
        auto newPasscodeConfirmed() -> PinLock::LockState;
        [[nodiscard]] auto getLock() -> PinLock &
        {
            return lock;
        }
        [[nodiscard]] auto getNewPasscodeHash() const noexcept -> unsigned int
        {
            return newPasscodeHash;
        }

      private:
        auto activateLock(
            std::function<void(PinLock::LockType type, const std::vector<unsigned int> &)> onActivatedCallback)
            -> PinLock::LockState;

        PinLock lock;
        unsigned int newPasscodeHash;
    };
} // namespace gui
