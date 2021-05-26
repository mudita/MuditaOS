// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include "gui/SwitchData.hpp"
#include "locks/widgets/Lock.hpp"

namespace locks
{
    using LockInput = const std::vector<unsigned int> &;

    enum class PhoneLockInputTypeAction
    {
        Unlock,
        Enable,
        Disable,
        ConfirmCurrent,
        Change,
        Set
    };

    enum class SimInputTypeAction
    {
        UnlockWithPin,
        UnlockWithPuk,
        ChangePin,
        EnablePin,
        DisablePin,
        Blocked,
        Error,
    };

    // class template that stores information that was sent along with switch message
    class LockData : public gui::SwitchData
    {
        Lock lock;
        PhoneLockInputTypeAction phoneLockInputTypeAction;

      public:
        explicit LockData(Lock lock,
                          PhoneLockInputTypeAction phoneLockInputTypeAction = PhoneLockInputTypeAction::Unlock)
            : SwitchData(), lock(std::move(lock)), phoneLockInputTypeAction(phoneLockInputTypeAction)
        {
            description = "LockPhoneData";
        }

        [[nodiscard]] auto getLock() const noexcept
        {
            return lock;
        }

        [[nodiscard]] auto getPhoneLockInputTypeAction() const noexcept
        {
            return phoneLockInputTypeAction;
        }
    };

    class SimLockData : public LockData
    {
        SimInputTypeAction simInputTypeAction;
        unsigned int errorCode;

      public:
        explicit SimLockData(Lock lock, SimInputTypeAction simInputTypeAction, unsigned int errorCode)
            : LockData(std::move(lock)), simInputTypeAction(simInputTypeAction), errorCode(errorCode)
        {
            description = "SimLockPhoneData";
        }

        [[nodiscard]] auto getSimInputTypeAction() const noexcept
        {
            return simInputTypeAction;
        }

        [[nodiscard]] auto getErrorCode() const noexcept
        {
            return errorCode;
        }
    };
} // namespace locks
