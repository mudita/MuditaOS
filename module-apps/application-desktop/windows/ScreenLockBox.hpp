// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PinLockBaseWindow.hpp"
#include "ScreenLockBaseBox.hpp"

namespace gui
{
    class ScreenLockBox : public ScreenLockBaseBox
    {
      public:
        ScreenLockBox(PinLockBaseWindow *LockBaseWindow) : ScreenLockBaseBox(LockBaseWindow), LockWindow(LockBaseWindow)
        {}

      private:
        void buildLockBox(unsigned int pinSize) final;
        void setVisibleStateBlocked() final;
        void setVisibleStateEnterPin(EnterPasscodeType type) final;
        void setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value) final;

        PinLockBaseWindow *LockWindow;
    };
} // namespace gui
