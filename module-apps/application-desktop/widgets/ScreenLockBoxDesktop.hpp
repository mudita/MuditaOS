// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "widgets/ScreenLockBox.hpp"
#include "application-desktop/windows/PinLockWindow.hpp"

namespace gui
{
    class ScreenLockBoxDesktop : public ScreenLockBox
    {
      public:
        ScreenLockBoxDesktop(PinLockBaseWindow *LockBaseWindow)
            : ScreenLockBox(LockBaseWindow), lockWindow(static_cast<PinLockWindow *>(LockBaseWindow))
        {}
        void buildLockBox(unsigned int pinSize) override;

      private:
        PinLockWindow *lockWindow;

        void setVisibleStateEnterPin(EnterPasscodeType type) final;
        void setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value) final;
        void setVisibleStateBlocked() final;
    };
} // namespace gui
