// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "locks/data/LockData.hpp"
#include "locks/windows/PinLockBaseWindow.hpp"
#include "PhoneLockBaseBox.hpp"

namespace gui
{
    class PhoneLockBox : public PhoneLockBaseBox
    {
      public:
        explicit PhoneLockBox(
            PinLockBaseWindow *LockBaseWindow,
            locks::PhoneLockInputTypeAction phoneLockInputTypeAction = locks::PhoneLockInputTypeAction::Unlock)
            : PhoneLockBaseBox(LockBaseWindow), LockWindow(LockBaseWindow)
        {
            applyLockActionText(phoneLockInputTypeAction);
        }

      private:
        void buildLockBox(unsigned int pinSize) final;
        void setVisibleStateBlocked() final;
        void setVisibleStateInputRequired(InputActionType type) final;
        void setVisibleStateInputInvalid(InputErrorType type, unsigned int value) final;
        void applyLockActionText(locks::PhoneLockInputTypeAction phoneLockInputTypeAction);

        [[nodiscard]] top_bar::Configuration configureTopBarLocked();
        [[nodiscard]] top_bar::Configuration configureTopBarUnLocked();

        PinLockBaseWindow *LockWindow;
        std::string textForInputRequired;
        std::string textForInvalidInput;
        std::string textForProvideNewInput;
        std::string textForConfirmNewInput;
        bool leftBottomBarState = false;
    };
} // namespace gui
