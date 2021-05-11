// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <utility>
#include "gui/SwitchData.hpp"
#include "locks/widgets/Lock.hpp"

namespace locks
{
    enum class PhoneLockInputTypeAction
    {
        Unlock,
        Enable,
        Disable,
        ConfirmCurrent,
        Change,
        Set
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

} // namespace locks
