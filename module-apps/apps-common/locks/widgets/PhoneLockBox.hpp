// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "locks/data/LockData.hpp"
#include "locks/windows/LockInputWindow.hpp"
#include "LockBoxConstantSize.hpp"

namespace gui
{
    class PhoneLockBox : public LockBoxConstantSize
    {
      public:
        explicit PhoneLockBox(
            LockInputWindow *LockBaseWindow,
            locks::PhoneLockInputTypeAction phoneLockInputTypeAction = locks::PhoneLockInputTypeAction::Unlock)
            : LockBoxConstantSize(LockBaseWindow), LockWindow(LockBaseWindow)
        {
            applyLockActionText(phoneLockInputTypeAction);
        }

      private:
        void buildLockBox(unsigned int pinSize) final;
        void setVisibleStateBlocked(const std::string &formattedTime) final;
        void setVisibleStateError(unsigned int errorCode) final;
        void setVisibleStateInputRequired(InputActionType type) final;
        void setVisibleStateInputInvalid(InputErrorType type, unsigned int value) final;
        void applyLockActionText(locks::PhoneLockInputTypeAction phoneLockInputTypeAction);

        LockInputWindow *LockWindow;
        std::string textForInputRequired;
        std::string textForInvalidInput;
        std::string textForProvideNewInput;
        std::string textForConfirmNewInput;
        bool leftNavBarState = false;
    };
} // namespace gui
