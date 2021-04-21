// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/lock-windows/PinLockBaseWindow.hpp"
#include "PhoneLockBaseBox.hpp"

namespace gui
{
    class PhoneLockBox : public PhoneLockBaseBox
    {
      public:
        PhoneLockBox(PinLockBaseWindow *LockBaseWindow) : PhoneLockBaseBox(LockBaseWindow), LockWindow(LockBaseWindow)
        {}

      private:
        void buildLockBox(unsigned int pinSize) final;
        void setVisibleStateBlocked() final;
        void setVisibleStateInputRequired(InputActionType type) final;
        void setVisibleStateInputInvalid(InputErrorType type, unsigned int value) final;

        PinLockBaseWindow *LockWindow;
    };
} // namespace gui
